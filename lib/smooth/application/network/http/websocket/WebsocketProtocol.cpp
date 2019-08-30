// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <smooth/application/network/http/websocket/WebsocketProtocol.h>
#include <smooth/core/util/string_util.h>
#include <smooth/application/network/http/regular/HTTPHeaderDef.h>
#include <smooth/application/network/http/regular/RegularHTTPProtocol.h>
#include <smooth/application/network/http/regular/responses/ErrorResponse.h>
#include <smooth/core/network/util.h>

namespace smooth::application::network::http::websocket
{
    using namespace smooth::application::network::http::regular;
    using namespace smooth::core::network;
    using namespace smooth::core;

    int WebsocketProtocol::get_wanted_amount(HTTPPacket& packet)
    {
        if (amount_wanted_in_current_state == 0)
        {
            data_received_in_current_state = 0;

            if (state == State::Header
                || state == State::ExtendedPayloadLength_2)
            {
                amount_wanted_in_current_state = 2;
            }
            else if (state == State::ExtendedPayloadLength_8)
            {
                amount_wanted_in_current_state = 8;
            }
            else if (state == State::MaskingKey)
            {
                amount_wanted_in_current_state = sizeof(frame_data.mask_key);
            }
            else
            {
                // Websockets can use up to 64 bits (well, 63 since MSB MUST always be 0,
                // see https://tools.ietf.org/html/rfc6455#section-5.2) to specify the size.
                // Since we can't handle that large data buffers, we split it into smaller parts based on
                // content_chunk_size

                auto remaining_payload = payload_length - received_payload;

                auto remaining_to_fill_current =
                    static_cast<decltype(received_payload_in_current_package)>(content_chunk_size)
                    - received_payload_in_current_package;

                auto amount_left = std::min(remaining_payload, remaining_to_fill_current);
                amount_wanted_in_current_state = static_cast<decltype(amount_wanted_in_current_state)>(amount_left);

                packet.expand_by(static_cast<int>(amount_left));
            }
        }

        return amount_wanted_in_current_state;
    }

    uint8_t* WebsocketProtocol::get_write_pos(HTTPPacket& packet)
    {
        if (state == State::Header)
        {
            return frame_data.header + data_received_in_current_state;
        }
        else if (state == State::ExtendedPayloadLength_2 || state == State::ExtendedPayloadLength_8)
        {
            return reinterpret_cast<uint8_t*>(&frame_data.ext_len) + data_received_in_current_state;
        }
        else if (state == State::MaskingKey)
        {
            return frame_data.mask_key + data_received_in_current_state;
        }
        else
        {
            return packet.data().data() + data_received_in_current_state;
        }
    }

    void WebsocketProtocol::data_received(HTTPPacket& packet, int length)
    {
        amount_wanted_in_current_state -= length;
        data_received_in_current_state += length;

        if (amount_wanted_in_current_state == 0)
        {
            // All requested data received
            if (state == State::Header)
            {
                op_code = static_cast<OpCode>(get_opcode());

                payload_length = get_initial_payload_length();

                if (payload_length == 126)
                {
                    state = State::ExtendedPayloadLength_2;
                }
                else if (payload_length == 127)
                {
                    state = State::ExtendedPayloadLength_8;
                }
                else if (is_data_masked())
                {
                    state = State::MaskingKey;
                }
                else
                {
                    state = State::Payload;
                }
            }
            else if (state == State::ExtendedPayloadLength_2)
            {
                payload_length = ntoh(frame_data.ext_len.len_16);
                state = State::MaskingKey;
            }
            else if (state == State::ExtendedPayloadLength_8)
            {
                payload_length = ntoh(frame_data.ext_len.len_64);
                state = State::MaskingKey;
            }
            else if (state == State::MaskingKey)
            {
                state = State::Payload;
            }
            else
            {
                update_received_payload(length);
            }

            if (is_complete(packet))
            {
                // Resize buffer to deliver exactly the number of received bytes to the application.
                using vector_type = std::remove_reference<decltype(packet.data())>::type;
                packet.data().resize(static_cast<vector_type::size_type>(received_payload_in_current_package));

                // De-mask data
                if (is_data_masked())
                {
                    for (decltype(packet.data().size()) i = 0; i < packet.data().size(); ++i, ++demask_ix)
                    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
                        packet.data()[i] = packet.data()[i] ^ frame_data.mask_key[demask_ix % 4];
#pragma GCC diagnostic pop
                    }
                }

                set_message_properties(packet);
            }
        }
        else if (state == State::Payload)
        {
            update_received_payload(length);
        }
    }

    void WebsocketProtocol::update_received_payload(int length)
    {
        auto len = static_cast<decltype(received_payload_in_current_package)>(length);
        received_payload += len;
        received_payload_in_current_package += len;
    }

    OpCode WebsocketProtocol::get_opcode() const
    {
        return static_cast<OpCode>(frame_data.header[0] & 0x0F);
    }

    int WebsocketProtocol::is_data_masked() const
    {
        return frame_data.header[1] & 0x80;
    }

    uint64_t WebsocketProtocol::get_initial_payload_length() const
    {
        return frame_data.header[1] & 0x7F;
    }

    bool WebsocketProtocol::is_complete(HTTPPacket& /*packet*/) const
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

        return state == State::Payload
               && (received_payload == payload_length
                   || received_payload_in_current_package ==
                   static_cast<decltype(received_payload_in_current_package)>(content_chunk_size));
#pragma GCC diagnostic pop
    }

    bool WebsocketProtocol::is_error()
    {
        return error;
    }

    void WebsocketProtocol::packet_consumed()
    {
        received_payload_in_current_package = 0;

        if (received_payload >= payload_length)
        {
            // All parts received
            state = State::Header;
            received_payload = 0;
            data_received_in_current_state = 0;
            demask_ix = 0;
            amount_wanted_in_current_state = 0;
        }
    }

    void WebsocketProtocol::reset()
    {
        state = State::Header;
        error = false;
        data_received_in_current_state = 0;
        payload_length = 0;
        received_payload = 0;
        received_payload_in_current_package = 0;
        demask_ix = 0;
        amount_wanted_in_current_state = 0;
    }

    void WebsocketProtocol::set_message_properties(HTTPPacket& packet)
    {
        if (received_payload <= static_cast<decltype(received_payload)>(content_chunk_size))
        {
            // This is the first part of the fragment we're forwarding to the application.
            if (received_payload < payload_length)
            {
                // There is more to come
                packet.set_continued();
            }
        }
        else
        {
            // Second or later fragment
            packet.set_continuation();

            if (received_payload < payload_length)
            {
                // Still more fragments coming.
                packet.set_continued();
            }
        }

        packet.set_ws_control_code(op_code);
    }
}

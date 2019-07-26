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
    using namespace smooth::core;
    using namespace smooth::core::network;
    using namespace smooth::application::network::http::regular;

    int WebsocketProtocol::get_wanted_amount(HTTPPacket& packet)
    {
        int len;
        if (state == State::Header)
        {
            len = 2;
        }
        else if (state == State::ExtendedPayloadLength_2)
        {
            len = 2;
        }
        else if (state == State::ExtendedPayloadLength_8)
        {
            len = 8;
        }
        else if (state == State::MaskingKey)
        {
            len = 4;
        }
        else
        {
            // Websockets can use up to 64 bits (well, 63 since MSB MUST always be 0,
            // see https://tools.ietf.org/html/rfc6455#section-5.2) to specify the size.
            // Since we can't handle that large data buffers, we split it into smaller parts based on content_chunk_size

            // First get the maximum number of bytes to read, this limits the value to fit in the type of
            // content_chunk_size, making the following cast safe too.
            auto size = std::min(payload_length - received_payload,
                                 static_cast<decltype(payload_length)>(content_chunk_size));
            len = static_cast<decltype(content_chunk_size)>(size);
            // Actual space needed is what whe already have, plus what we want to read.
            auto space_needed = static_cast<decltype(content_chunk_size)>(received_payload_in_current_package) + len;
            packet.ensure_room(space_needed);
        }

        return len;
    }

    void WebsocketProtocol::data_received(HTTPPacket& packet, int length)
    {
        total_byte_count += length;

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
            auto len = static_cast<decltype(received_payload_in_current_package)>(length);
            if (is_data_masked())
            {
                for (auto i = 0u; i < len; ++i)
                {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
                    packet.data()[i] = packet.data()[received_payload_in_current_package + i] ^
                                       (frame_data.mask_key[(received_payload + i) % 4]);
#pragma GCC diagnostic pop
                }
            }

            received_payload += len;
            received_payload_in_current_package += len;
        }

        if (is_complete(packet))
        {
            // Resize buffer to deliver exactly the number of received bytes to the application.
            using vector_type = std::remove_reference<decltype(packet.data())>::type;
            packet.data().resize(static_cast<vector_type::size_type>(received_payload_in_current_package));

            set_message_properties(packet);
        }
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

    uint8_t* WebsocketProtocol::get_write_pos(HTTPPacket& packet)
    {
        if (state == State::Header)
        {
            return frame_data.header + total_byte_count;
        }
        else if (state == State::ExtendedPayloadLength_2 || state == State::ExtendedPayloadLength_8)
        {
            return reinterpret_cast<uint8_t*>(&frame_data.ext_len);
        }
        else if (state == State::MaskingKey)
        {
            return frame_data.mask_key;
        }
        else
        {
            return packet.data().data() + received_payload_in_current_package;
        }
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
        error = false;
        received_payload_in_current_package = 0;

        if (error || received_payload >= payload_length)
        {
            // All parts received
            received_payload = 0;
            total_byte_count = 0;
            state = State::Header;
        }
    }

    void WebsocketProtocol::reset()
    {
        state = State::Header;
        error = false;
        total_byte_count = 0;
        payload_length = 0;
        received_payload = 0;
        received_payload_in_current_package = 0;
    }

    void WebsocketProtocol::set_message_properties(HTTPPacket& packet)
    {
        if(received_payload <= static_cast<decltype(received_payload)>(content_chunk_size))
        {
            // This is the first part of the fragment we're forwarding to the application.
            if(received_payload < payload_length)
            {
                // There is more to come
                packet.set_continued();
            }
        }
        else
        {
            // Second or later fragment
            packet.set_continuation();

            if(received_payload < payload_length)
            {
                // Still more fragments coming.
                packet.set_continued();
            }
        }

        packet.set_ws_control_code(op_code);
    }

    bool WebsocketProtocol::is_fin_frame() const
    {
        return frame_data.header[0] & 0x80;
    }
}
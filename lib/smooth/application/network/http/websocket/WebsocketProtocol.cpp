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

// https://tools.ietf.org/html/rfc6455#section-5.2

/*
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-------+-+-------------+-------------------------------+
 |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
 |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
 |N|V|V|V|       |S|             |   (if payload len==126/127)   |
 | |1|2|3|       |K|             |                               |
 +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
 |     Extended payload length continued, if payload len == 127  |
 + - - - - - - - - - - - - - - - +-------------------------------+
 |                               |Masking-key, if MASK set to 1  |
 +-------------------------------+-------------------------------+
 | Masking-key (continued)       |          Payload Data         |
 +-------------------------------- - - - - - - - - - - - - - - - +
 :                     Payload Data continued ...                :
 + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 |                     Payload Data continued ...                |
 +---------------------------------------------------------------+

 */

namespace smooth::application::network::http::websocket
{
    using namespace smooth::core;
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
            len = 2;
        }
        else if (state == State::MaskingKey)
        {
            len = 4;
        }
        else
        {
            packet.ensure_room(payload_length);
            len = payload_length - received_payload;
        }

        return len;
    }

    void WebsocketProtocol::data_received(HTTPPacket& packet, int length)
    {
        total_byte_count += length;

        if (state == State::Header)
        {
            std::bitset<8> first(frame_data[0]);
            fin = first[7];
            RSV1 = first[6];
            RSV2 = first[5];
            RSV3 = first[4];
            op_code = static_cast<OpCode>((frame_data[0] >> 1) & 0x0F);

            std::bitset<8> second(frame_data[1]);

            masked = second[7];
            payload_length = frame_data[1] & 0x7F;

            if (payload_length == 126)
            {
                state = State::ExtendedPayloadLength_2;
            }
            else if (payload_length == 127)
            {
                // Can't handle such large data.
                Log::error("WebsocketProtocol", "Too large data size");
                error = true;
            }
            else if (masked)
            {
                state = State::MaskingKey;
            }
            else
            {
                decode_index = 0;
                state = State::Payload;
            }
        }
        else if (state == State::ExtendedPayloadLength_2)
        {
            auto len = reinterpret_cast<uint16_t*>(&frame_data[2]);
            payload_length = *len;
        }
        else if (state == State::ExtendedPayloadLength_8)
        {
            //auto len = reinterpret_cast<uint64_t*>(&frame_data[2]);
            //payload_length = *len;
        }
        else if (state == State::MaskingKey)
        {
            decode_index = 0;
            state = State::Payload;
        }
        else
        {
            if (masked)
            {
                for (auto i = static_cast<unsigned long>(received_payload); i < static_cast<unsigned long>(received_payload + length); ++i)
                {
                    packet.data()[i] = packet.data()[i] ^ mask_key[i % 4];
                }
            }
            received_payload += length;
        }
    }

    uint8_t* WebsocketProtocol::get_write_pos(HTTPPacket& packet)
    {
        if (state == State::Header
            || state == State::ExtendedPayloadLength_2
            || state == State::ExtendedPayloadLength_8)
        {
            return frame_data.data() + total_byte_count;
        }
        else if (state == State::MaskingKey)
        {
            return mask_key.data();
        }
        else
        {
            return packet.data().data() + received_payload;
        }
    }

    bool WebsocketProtocol::is_complete(HTTPPacket& /*packet*/) const
    {
        return state == State::Payload && received_payload == payload_length;
    }

    bool WebsocketProtocol::is_error()
    {
        return error;
    }

    void WebsocketProtocol::packet_consumed()
    {
        error = false;
    }

    void WebsocketProtocol::reset()
    {
        state = State::Header;
        error = false;
        total_byte_count = 0;
        payload_length = 0;
        received_payload = 0;
    }

}
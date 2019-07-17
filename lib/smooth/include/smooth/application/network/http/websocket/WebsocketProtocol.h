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

#pragma once

#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/application/network/http/IServerResponse.h>
#include "OpCode.h"

namespace smooth::application::network::http
{
    class HTTPPacket;
}

namespace smooth::application::network::http::websocket
{
    class WebsocketProtocol
            : public smooth::core::network::IPacketAssembly<WebsocketProtocol, smooth::application::network::http::HTTPPacket>
    {
            using packet_type = smooth::application::network::http::HTTPPacket;
        public:
            WebsocketProtocol(int content_chunk_size, IServerResponse& response)
                    : content_chunk_size(content_chunk_size),
                      response(response)
            {
            }

            int get_wanted_amount(HTTPPacket& packet) override;

            void data_received(HTTPPacket& packet, int length) override;

            uint8_t* get_write_pos(HTTPPacket& packet) override;

            bool is_complete(HTTPPacket& packet) const override;

            bool is_error() override;

            void packet_consumed() override;

            void reset() override;

            /* https://tools.ietf.org/html/rfc6455#section-5.2

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
             +---------------------------------------------------------------+ */

#pragma pack(push, 1)
            struct FrameData
            {
                uint8_t header[2];
                union
                {
                    uint16_t len_16;
                    uint64_t len_64;
                } ext_len;
                uint8_t mask_key[4];
            };

            FrameData frame_data{};
#pragma pack(pop)

        private:
            enum class State
            {
                    Header,
                    ExtendedPayloadLength_2,
                    ExtendedPayloadLength_8,
                    MaskingKey,
                    Payload
            };

            State state{State::Header};

            int content_chunk_size;
            OpCode op_code{OpCode::Continuation};
            IServerResponse& response;

            bool error{false};
            int total_byte_count{0};
            uint64_t payload_length{0};
            uint64_t received_payload{0};
            uint64_t received_payload_in_current_package{0};

            void set_message_properties(HTTPPacket& packet);

            uint64_t get_initial_payload_length() const;

            int is_data_masked() const;

            OpCode get_opcode() const;

            bool is_fin_frame() const;
    };
}
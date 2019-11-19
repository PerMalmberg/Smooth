/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include "smooth/core/network/IPacketAssembly.h"
#include "smooth/application/network/http/IServerResponse.h"
#include "OpCode.h"

namespace smooth::application::network::http
{
    class HTTPPacket;
}

namespace smooth::application::network::http::websocket
{
    class WebsocketProtocol
        : public smooth::core::network::IPacketAssembly<WebsocketProtocol,
                                                        smooth::application::network::http::HTTPPacket>
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
                }

                ext_len{ 0 };
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

            State state{ State::Header };

            int content_chunk_size;
            OpCode op_code{ OpCode::Continuation };
            IServerResponse& response;

            bool error{ false };
            int data_received_in_current_state{ 0 };
            uint64_t payload_length{ 0 };
            uint64_t received_payload{ 0 };
            uint64_t demask_ix{ 0 };
            uint64_t received_payload_in_current_package{ 0 };
            int amount_wanted_in_current_state = 0;

            void set_message_properties(HTTPPacket& packet);

            [[nodiscard]] uint64_t get_initial_payload_length() const;

            [[nodiscard]] int is_data_masked() const;

            [[nodiscard]] OpCode get_opcode() const;

            void update_received_payload(int length);
    };
}

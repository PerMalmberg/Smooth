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

#include <array>
#include "smooth/core/network/IPacketDisassembly.h"
#include "smooth/core/network/IPacketAssembly.h"
#include "StreamPacket.h"

namespace secure_server_socket_test
{
    class StreamingProtocol
        : public smooth::core::network::IPacketAssembly<StreamingProtocol, StreamPacket>
    {
        public:
            using packet_type = StreamPacket;

            int get_wanted_amount(StreamPacket& /*packet*/) override
            {
                return 1;
            }

            void data_received(StreamPacket& /*packet*/, int /*length*/) override
            {
                complete = true;
            }

            uint8_t* get_write_pos(StreamPacket& packet) override
            {
                return packet.data().data();
            }

            bool is_complete(StreamPacket& /*packet*/) const override
            {
                return complete;
            }

            bool is_error() override
            {
                // Can't fail when there really is no actual protocol.
                return false;
            }

            void packet_consumed() override
            {
                complete = false;
            }

            void reset() override
            {
                packet_consumed();
            }

        private:
            bool complete{ false };
    };
}

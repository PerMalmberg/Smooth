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

#include <array>
#include <smooth/core/network/IPacketDisassembly.h>
#include <smooth/core/network/IPacketAssembly.h>
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
            bool complete{false};
    };
}
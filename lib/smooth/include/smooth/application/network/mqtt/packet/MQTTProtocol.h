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

#include <vector>
#include <smooth/core/util/ByteSet.h>
#include <smooth/core/network/IPacketAssembly.h>
#include <smooth/core/network/IPacketDisassembly.h>
#include <smooth/application/network/mqtt/MQTTProtocolDefinitions.h>
#include <smooth/application/network/mqtt/packet/PacketIdentifierFactory.h>
#include <smooth/application/network/mqtt/packet/MQTTPacket.h>

namespace smooth::application::network::mqtt::packet
{
    class IPacketReceiver;

    class MQTTProtocol
        : public smooth::core::network::IPacketAssembly<MQTTProtocol, MQTTPacket>
    {
        public:
            using packet_type = MQTTPacket;

            // Must return the number of bytes the packet wants to fill
            // its internal buffer, e.g. header, checksum etc. Returned
            // value will differ depending on how much data already has been provided.
            int get_wanted_amount(MQTTProtocol::packet_type& packet) override;

            // Used by the underlying framework to notify the packet that {length} bytes
            // has been written to the buffer pointed to by get_write_pos().
            // During the call to this method the packet should do whatever it needs to
            // evaluate if it needs more data or if it is complete.
            void data_received(MQTTProtocol::packet_type& packet, int length) override;

            // Must return the current write position of the internal buffer.
            // Must point to a buffer than can accept the number of bytes returned by
            // get_wanted_amount().
            uint8_t * get_write_pos(MQTTProtocol::packet_type& packet) override;

            // Must return true when the packet has received all data it needs
            // to fully assemble.
            bool is_complete(MQTTProtocol::packet_type& packet) const override;

            // Must return true whenever the packet is unable to correctly assemble
            // based on received data.
            bool is_error() override;

            bool is_too_big() const;

            void packet_consumed() override;

            void reset() override;

        private:
            enum ReadingHeaderSection
            {
                START = 1,
                REMAINING_LENGTH,
                DATA
            };

            ReadingHeaderSection state = ReadingHeaderSection::START;
            int bytes_received = 0;
            int remaining_bytes_to_read = 1;
            int received_header_length = 0;
            bool error = false;
            bool too_big = false;
    };
}

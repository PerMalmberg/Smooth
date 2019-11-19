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

#include <vector>
#include "smooth/core/util/ByteSet.h"
#include "smooth/core/network/IPacketAssembly.h"
#include "smooth/core/network/IPacketDisassembly.h"
#include "smooth/application/network/mqtt/MQTTProtocolDefinitions.h"
#include "smooth/application/network/mqtt/packet/PacketIdentifierFactory.h"
#include "smooth/application/network/mqtt/packet/MQTTPacket.h"

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
            uint8_t* get_write_pos(MQTTProtocol::packet_type& packet) override;

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

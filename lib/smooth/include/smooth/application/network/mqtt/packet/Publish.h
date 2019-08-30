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

#include <string>
#include <algorithm>
#include <smooth/application/network/mqtt/packet/MQTTProtocol.h>

namespace smooth::application::network::mqtt::packet
{
    class Publish
        : public MQTTPacket
    {
        public:
            Publish() = default;

            explicit Publish(const MQTTPacket& packet)
                    : MQTTPacket(packet)
            {
            }

            Publish(const std::string& topic, const uint8_t* data, int length, QoS qos, bool retain);

            void visit(IPacketReceiver& receiver) override;

            uint16_t get_packet_identifier() const override
            {
                uint16_t id = 0;

                if (has_packet_identifier())
                {
                    calculate_remaining_length_and_variable_header_offset();
                    auto pos = get_variable_header_start() + get_variable_header_length() - 2;
                    id = read_packet_identifier(pos);
                }

                return id;
            }

            std::string get_topic() const;

            std::vector<uint8_t>::const_iterator get_payload_cbegin() const override
            {
                return get_variable_header_start() + get_variable_header_length();
            }

            std::vector<uint8_t>::const_iterator get_payload_cend() const
            {
                return data.cend();
            }

        protected:
            bool has_packet_identifier() const override
            {
                return get_qos() > QoS::AT_MOST_ONCE;
            }

            bool has_payload() const override
            {
                // Payload is optional for Publish
                return get_payload_length() > 0;
            }

            int get_variable_header_length() const override;
    };
}

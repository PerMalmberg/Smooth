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

#include <string>
#include <algorithm>
#include "smooth/application/network/mqtt/packet/MQTTProtocol.h"

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

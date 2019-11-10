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

#include "smooth/application/network/mqtt/packet/MQTTProtocol.h"
#include "smooth/application/network/mqtt/packet/PacketIdentifierFactory.h"

namespace smooth::application::network::mqtt::packet
{
    class Subscribe
        : public MQTTPacket
    {
        public:
            Subscribe() = default;

            explicit Subscribe(const MQTTPacket& packet)
                    : MQTTPacket(packet)
            {
            }

            Subscribe(const std::string& topic, QoS qos)
            {
                set_header(SUBSCRIBE, 0x2);
                std::vector<uint8_t> data;
                append_msb_lsb(PacketIdentifierFactory::get_id(), data);
                append_string(topic, data);
                data.push_back(qos);
                apply_constructed_data(data);
            }

            uint16_t get_packet_identifier() const override
            {
                return read_packet_identifier(get_variable_header_start());
            }

            void get_topics(std::vector<std::pair<std::string, QoS>>& topics) const;

            void visit(IPacketReceiver& receiver) override;

        protected:
            int get_variable_header_length() const override
            {
                return 2;
            }

            bool has_packet_identifier() const override
            {
                return true;
            }

            bool has_payload() const override
            {
                return true;
            }
    };
}

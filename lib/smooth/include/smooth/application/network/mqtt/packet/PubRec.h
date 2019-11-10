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

namespace smooth::application::network::mqtt::packet
{
    class PubRec
        : public MQTTPacket
    {
        public:
            PubRec() = default;

            explicit PubRec(uint16_t packet_id)
            {
                set_header(PUBREC, 0);
                std::vector<uint8_t> variable_header;
                append_msb_lsb(packet_id, variable_header);
                apply_constructed_data(variable_header);
            }

            explicit PubRec(const MQTTPacket& packet)
                    : MQTTPacket(packet)
            {
            }

            void visit(IPacketReceiver& receiver) override;

            uint16_t get_packet_identifier() const override;

        protected:
            bool has_packet_identifier() const override
            {
                return true;
            }

            int get_variable_header_length() const override
            {
                // Only packet identifier in this variable header
                return 2;
            }
    };
}

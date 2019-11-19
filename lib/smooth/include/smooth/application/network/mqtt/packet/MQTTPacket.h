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

#include <stdint.h>
#include <vector>
#include "smooth/application/network/mqtt/Logging.h"
#include "smooth/core/util/ByteSet.h"
#include "smooth/application/network/mqtt/MQTTProtocolDefinitions.h"
#include "smooth/core/network/IPacketDisassembly.h"

namespace smooth::application::network::mqtt::packet
{
    class IPacketReceiver;

    class MQTTPacket
        : public smooth::core::network::IPacketDisassembly
    {
        friend class MQTTProtocol;
        public:
            ~MQTTPacket() override = default;

            virtual std::vector<uint8_t>::const_iterator get_payload_cbegin() const
            {
                return data.cend();
            }

            virtual void visit(IPacketReceiver& receiver);

            virtual uint16_t get_packet_identifier() const
            {
                return 0;
            }

            QoS get_qos() const;

            bool validate_packet() const;

            long get_payload_length() const;

            void dump(const char* header) const;

            PacketType get_mqtt_type() const;

            bool is_too_big() const
            { return too_big; }

            void set_dup_flag();

            const char* get_mqtt_type_as_string() const;

            int get_send_length() override
            { return static_cast<int>(data.size()); }

            const uint8_t* get_data() override
            { return data.data(); }
        protected:
            std::string get_string(std::vector<uint8_t>::const_iterator offset) const;

            void append_data(const uint8_t* data, int length, std::vector<uint8_t>& target);

            uint16_t read_packet_identifier(std::vector<uint8_t>::const_iterator pos) const
            {
                return static_cast<uint16_t>(*pos << 8 | *(pos + 1));
            }

            virtual bool has_packet_identifier() const
            {
                return false;
            }

            virtual bool has_payload() const
            {
                return false;
            }

            virtual int get_variable_header_length() const
            {
                return 0;
            }

            void set_header(PacketType type, QoS qos, bool dup, bool retain);

            void set_header(PacketType type, uint8_t flags);

            void append_string(const std::string& str, std::vector<uint8_t>& target);

            void append_msb_lsb(uint16_t value, std::vector<uint8_t>& target);

            void apply_constructed_data(const std::vector<uint8_t>& variable);

            void encode_remaining_length(int length);

            int calculate_remaining_length_and_variable_header_offset() const;

            std::vector<uint8_t>::const_iterator get_variable_header_start() const
            {
                return data.cbegin() + variable_header_start_ix;
            }

            std::vector<uint8_t> data{};
            mutable long variable_header_start_ix = 0;
            mutable bool error = false;
            bool too_big = false;
    };
}

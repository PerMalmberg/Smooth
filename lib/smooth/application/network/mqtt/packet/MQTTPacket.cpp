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

#include <sstream>
#include "smooth/application/network/mqtt/packet/MQTTPacket.h"
#include "smooth/core/logging/log.h"
#include "smooth/application/network/mqtt/packet/IPacketReceiver.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::application::network::mqtt::packet
{
    void MQTTPacket::append_data(const uint8_t* data, int length, std::vector<uint8_t>& target)
    {
        for (int i = 0; i < length; ++i)
        {
            target.push_back(data[i]);
        }
    }

    std::string MQTTPacket::get_string(std::vector<uint8_t>::const_iterator offset) const
    {
        auto length = static_cast<uint16_t>((*offset) << 8);
        ++offset;
        length = static_cast<uint16_t>(length | *offset);
        ++offset;

        std::stringstream ss;

        for (int i = 0; offset != data.cend() && i < length; ++offset, ++i)
        {
            ss << *offset;
        }

        return ss.str();
    }

    QoS MQTTPacket::get_qos() const
    {
        // QoS is always located in the first byte but not all packets
        // actually use the bits as QoS (e.g. PubRel)
        smooth::core::util::ByteSet b(data[0]);
        auto value = static_cast<uint8_t>((b.test(1) ? 1 : 0) | ((b.test(2) ? 1 : 0) << 1));

        return static_cast<QoS>(value);
    }

    void MQTTPacket::set_header(PacketType type, QoS qos, bool dup, bool retain)
    {
        smooth::core::util::ByteSet flags(0);
        flags.set(0, retain);
        flags.set(1, qos & 0x1);
        flags.set(2, qos & 0x2);
        flags.set(3, dup);

        set_header(type, flags);
    }

    void MQTTPacket::set_header(PacketType type, uint8_t flags)
    {
        auto value = static_cast<uint8_t>((type << 4) | flags);
        data.push_back(value);
    }

    void MQTTPacket::append_string(const std::string& str, std::vector<uint8_t>& target)
    {
        // Maximum length is 65535 since that is what can be represented as a 16-bit number.
        auto length = static_cast<uint16_t>(str.length());
        append_msb_lsb(length, target);

        for (uint16_t i = 0; i < length; ++i)
        {
            target.push_back(static_cast<uint8_t &&>(str[i]));
        }
    }

    void MQTTPacket::append_msb_lsb(uint16_t value, std::vector<uint8_t>& target)
    {
        target.push_back(static_cast<uint8_t &&>(value >> 8));
        target.push_back(static_cast<uint8_t &&>(value & 0xFF));
    }

    void MQTTPacket::apply_constructed_data(const std::vector<uint8_t>& variable)
    {
        encode_remaining_length(static_cast<int>(variable.size()));

        // Using move_iterator we reduce the memory foot print by actually moving
        // instead of copying the data.
        std::copy(std::make_move_iterator(variable.begin()),
                  std::make_move_iterator(variable.end()),
                  std::back_inserter(data));
        calculate_remaining_length_and_variable_header_offset();
    }

    void MQTTPacket::encode_remaining_length(int length)
    {
        if (length > 0)
        {
            for (int i = 0; i < 4 && length > 0; ++i)
            {
                data.push_back(static_cast<uint8_t &&>(length % 0x80));
                length /= 0x80;

                if (length > 0)
                {
                    data[data.size() - 1] |= 0x80;
                }
            }
        }
        else
        {
            data.push_back(0);
        }
    }

    int MQTTPacket::calculate_remaining_length_and_variable_header_offset() const
    {
        int res = 0;

        bool done = false;
        int multiplier = 1;

        auto curr = data.cbegin() + 1;

        for (int i = 0; curr != data.cend() && !done && !error; ++curr, ++i)
        {
            res += (*curr & 0x7F) * multiplier;
            multiplier *= 128;
            core::util::ByteSet b(*curr);
            done = !b.test(7);

            // If we've calculated four items and still not marked as done the
            // remaining length is malformed.
            error = !done && i == 3;
        }

        // If present, variable header start always is located after the remaining bytes
        variable_header_start_ix = std::distance(data.cbegin(), curr);

        if (error)
        {
            smooth::core::logging::Log::error(mqtt_log_tag, "Invalid remaining length");
        }

        return res;
    }

    bool MQTTPacket::validate_packet() const
    {
        // Must first check if the pack was deemed to big. If that is the
        // case, then the data held by the packet is invalid and must not
        // be used for calculations since the data has been overwritten
        // at least once while reading the data we cannot hold.

        bool res = true;

        if (too_big)
        {
            res = false;
        }
        else
        {
            // Ensure that data lengths add up.
            calculate_remaining_length_and_variable_header_offset();
            auto left_over = static_cast<long>(data.size())
                             - std::distance(data.cbegin(), get_variable_header_start()) // Fixed header
                             - static_cast<long>(get_variable_header_length()) // Variable header
                             - get_payload_length(); // Payload

            if (left_over != 0)
            {
                Log::error(mqtt_log_tag, "Invalid packet, lengths do not add up: {}", left_over);
                res = false;
            }
        }

        return res;
    }

    long MQTTPacket::get_payload_length() const
    {
        calculate_remaining_length_and_variable_header_offset();

        long payload_length = std::distance(
                get_variable_header_start() + get_variable_header_length(), data.cend());

        return payload_length;
    }

    void MQTTPacket::dump(const char* header) const
    {
        std::stringstream ss;
        calculate_remaining_length_and_variable_header_offset();

        ss << "[" << get_mqtt_type_as_string() << "] "
           << "Raw(" << data.size() << ") "
           << "Fix(" << std::distance(data.cbegin(), get_variable_header_start()) << ") "
           << "Var(" << get_variable_header_length() << ") "
           << "Pay(" << get_payload_length() << ") ";

        if (has_packet_identifier())
        {
            ss << "ID(" << get_packet_identifier() << ") ";
        }

        ss << "Q(" << static_cast<int>(get_qos()) << ") ";
        core::util::ByteSet b(data[0]);
        ss << "R(" << b.test(0) << ") ";
        ss << "D(" << b.test(3) << ") ";

        Log::verbose(mqtt_log_tag, "{}: {}", header, ss.str());

        if (has_payload() && get_payload_length() > 0)
        {
            ss.str("");

            for (auto p = get_payload_cbegin(); p != data.cend(); p++)
            {
                if (isprint(*p))
                {
                    ss << static_cast<char>(*p);
                }
                else
                {
                    ss << std::hex << static_cast<int>(*p) << " ";
                }
            }

            Log::verbose(mqtt_log_tag, "{}: {}", header, ss.str());
        }
    }

    const char* MQTTPacket::get_mqtt_type_as_string() const
    {
        auto it = packet_type_as_string.find(get_mqtt_type());

        return it == packet_type_as_string.end() ? "Unknown packet" : it->second;
    }

    PacketType MQTTPacket::get_mqtt_type() const
    {
        return static_cast<PacketType>((data[0] & 0xFF) >> 4);
    }

    void MQTTPacket::visit(IPacketReceiver& receiver)
    {
        receiver.receive(*this);
    }

    void MQTTPacket::set_dup_flag()
    {
        core::util::ByteSet b(data[0]);
        b.set(3, true);
        data[0] = b;
    }
}

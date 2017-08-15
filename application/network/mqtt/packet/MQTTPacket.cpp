//
// Created by permal on 7/15/17.
//

#include <bitset>
#include <sstream>
#include <smooth/application/network/mqtt/packet/MQTTPacket.h>
#include "esp_log.h"
#include "sdkconfig.h"

using namespace std;

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                namespace packet
                {
                    PacketType MQTTPacket::get_mqtt_type() const
                    {
                        return static_cast<PacketType>((packet[0] & 0xFF) >> 4);
                    }

                    const char* MQTTPacket::get_mqtt_type_as_string() const
                    {
                        auto it = packet_type_as_string.find(get_mqtt_type());
                        return it == packet_type_as_string.end() ? "Unknown packet" : it->second;
                    }

                    int MQTTPacket::get_wanted_amount()
                    {
                        // If we're just reading data to get rid of a too large packet
                        // then we read the maximum allowed amount, but it will just be overwritten
                        // by the next time we read data.
                        int wanted;
                        if (is_too_big())
                        {
                            wanted = std::min( wanted_amount, CONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE);
                        }
                        else
                        {
                            wanted = wanted_amount;
                        }

                        return wanted;
                    }

                    void MQTTPacket::data_received(int length)
                    {
                        bytes_received += length;

                        if (state == START)
                        {
                            // First byte of the packet
                            state = REMAINING_LENGTH;
                        }
                        else if (state == REMAINING_LENGTH)
                        {
                            // Second and optionally 3rd to 5th bytes
                            core::util::ByteSet b(packet[bytes_received - 1]);
                            if (b.test(7))
                            {
                                // Not yet received all length bytes
                                wanted_amount = 1;
                            }
                            else
                            {
                                received_header_length = bytes_received;

                                // We can now calculate the length of the remaining data
                                wanted_amount = calculate_remaining_length_and_variable_header_offset();

                                if (wanted_amount > CONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE)
                                {
                                    ESP_LOGV("MQTTPacket", "Too big packet detected: %d > %d", wanted_amount,
                                             CONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE);
                                    state = DATA;
                                    too_big = true;
                                }
                                else if (wanted_amount > 0)
                                {
                                    state = DATA;
                                }
                            }
                        }
                        else if (state == DATA)
                        {
                            wanted_amount -= length;
                        }
                        else
                        {
                            error = true;
                        }
                    }

                    int MQTTPacket::calculate_remaining_length_and_variable_header_offset() const
                    {
                        int res = 0;

                        bool done = false;
                        int multiplier = 1;

                        auto curr = packet.cbegin() + 1;

                        for (int i = 0; curr != packet.cend() && !done && !error; ++curr, ++i)
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
                        variable_header_start = curr;

                        if (error)
                        {
                            ESP_LOGE("mqtt", "Invalid remaining length");
                        }

                        return res;
                    }

                    void MQTTPacket::encode_remaining_length(int length)
                    {
                        if (length > 0)
                        {
                            for (int i = 0; i < 4 && length > 0; ++i)
                            {
                                packet.push_back(length % 0x80);
                                length /= 0x80;

                                if (length > 0)
                                {
                                    packet[packet.size() - 1] |= 0x80;
                                }
                            }
                        }
                        else
                        {
                            packet.push_back(0);
                        }
                    }

                    void MQTTPacket::append_string(const std::string& str, std::vector<uint8_t>& target)
                    {
                        // Maximum length is 65535 since that is what can be represented as a 16-bit number.
                        uint16_t length = static_cast<uint16_t>( str.length());
                        append_msb_lsb(length, target);

                        for (uint16_t i = 0; i < length; ++i)
                        {
                            target.push_back(str[i]);
                        }
                    }

                    void MQTTPacket::append_msb_lsb(uint16_t value, std::vector<uint8_t>& target)
                    {
                        target.push_back(value >> 8);
                        target.push_back(value & 0xFF);
                    }

                    void MQTTPacket::append_data(const uint8_t* data, int length, std::vector<uint8_t>& target)
                    {
                        for (int i = 0; i < length; ++i)
                        {
                            target.push_back(data[i]);
                        }
                    }

                    void MQTTPacket::apply_constructed_data(const std::vector<uint8_t>& variable)
                    {
                        encode_remaining_length(variable.size());
                        std::copy(variable.begin(), variable.end(), std::back_inserter(packet));
                        calculate_remaining_length_and_variable_header_offset();
                    }

                    void MQTTPacket::set_header(PacketType type, QoS qos, bool dup, bool retain)
                    {
                        core::util::ByteSet flags(0);
                        flags.set(0, retain);
                        flags.set(1, qos & 0x1);
                        flags.set(2, qos & 0x2);
                        flags.set(3, dup);

                        set_header(type, flags);
                    }

                    void MQTTPacket::set_header(PacketType type, uint8_t flags)
                    {
                        uint8_t value = (type << 4) | flags;
                        packet.push_back(value);
                    }

                    void MQTTPacket::set_dup_flag()
                    {
                        core::util::ByteSet b(packet[0]);
                        b.set(3, true);
                        packet[0] = b;
                    }

                    std::string MQTTPacket::get_string(std::vector<uint8_t>::const_iterator offset) const
                    {
                        uint16_t length = (*offset) << 8;
                        offset++;
                        length |= *offset;
                        offset++;

                        std::stringstream ss;
                        for (int i = 0; offset != packet.cend() && i < length; offset++, i++)
                        {
                            ss << *offset;
                        }

                        return ss.str();
                    }

                    QoS MQTTPacket::get_qos() const
                    {
                        // QoS is always located in the first byte but not all packets
                        // actually use the bits as QoS (e.g. PubRel)
                        smooth::core::util::ByteSet b(packet[0]);
                        uint8_t value = (b.test(1) ? 1 : 0) | ((b.test(2) ? 1 : 0) << 1);
                        return static_cast<QoS>( value );
                    }

                    int MQTTPacket::get_payload_length() const
                    {
                        calculate_remaining_length_and_variable_header_offset();

                        int payload_length = std::distance(
                                variable_header_start + get_variable_header_length(),
                                packet.cend());

                        return payload_length;
                    }

                    void MQTTPacket::dump(const char* header) const
                    {
                        std::stringstream ss;
                        calculate_remaining_length_and_variable_header_offset();

                        ss << "[" << get_mqtt_type_as_string() << "] "
                           << "Raw(" << packet.size() << ") "
                           << "Fix(" << std::distance(packet.cbegin(), variable_header_start) << ") "
                           << "Var(" << get_variable_header_length() << ") "
                           << "Pay(" << get_payload_length() << ") ";

                        if (has_packet_identifier())
                        {
                            ss << "ID(" << get_packet_identifier() << ") ";
                        }

                        ss << "Q(" << static_cast<int>( get_qos()) << ") ";
                        core::util::ByteSet b(packet[0]);
                        ss << "R(" << b.test(0) << ") ";
                        ss << "D(" << b.test(3) << ") ";
                        ESP_LOGD(header, "%s", ss.str().c_str());


                        if (has_payload() && get_payload_length() > 0)
                        {
                            ss.str("");

                            for (auto b = get_payload_cbegin(); b != packet.cend(); b++)
                            {
                                if (isascii(*b))
                                {
                                    ss << static_cast<char>(*b);
                                }
                                else
                                {
                                    ss << std::hex << static_cast<int>(*b) << " ";
                                }
                            }

                            ESP_LOGD(header, "%s", ss.str().c_str());
                        }
                    }

                    bool MQTTPacket::validate_packet() const
                    {
                        // Must first check if the back was deemed to big. If that is the
                        // case, then the data held by the packet is invalid and must not
                        // be used for calculations since the data has been overwritten
                        // at least once while reading the data we cannot hold.

                        bool res = true;

                        if(is_too_big())
                        {
                            ESP_LOGE("MQTTPacket", "Packet is too big.");
                            res = false;
                        }
                        else
                        {
                            // Ensure that data lengths add up.
                            calculate_remaining_length_and_variable_header_offset();
                            int left_over = packet.size()
                                            // Fixed header
                                            - std::distance(packet.cbegin(), variable_header_start)
                                            // Variable header
                                            - get_variable_header_length()
                                            // Payload
                                            - get_payload_length();

                            if( left_over != 0 )
                            {
                                ESP_LOGE("MQTTPacket", "Invalid packet, lengths do not add up: %d", left_over);
                                res = false;
                            }
                        }

                        return res;
                    }

                    uint8_t* MQTTPacket::get_write_pos()
                    {
                        uint8_t* pos;

                        // Make room for remaining data. In the case of a too big package, allocate
                        // maximum allowed to enable quick read of the data.
                        auto required_size =
                                is_too_big() ?
                                received_header_length + CONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE :
                                bytes_received + get_wanted_amount();

                        // Make sure there is room to do direct memory writes by expanding the vector.
                        while (packet.size() < required_size)
                        {
                            packet.push_back(0);
                        }

                        if (is_too_big())
                        {
                            // Always write to the byte after the header
                            pos = &packet[received_header_length];
                        }
                        else
                        {
                            // Append to the already received data.
                            pos = &packet[bytes_received];
                        }

                        return pos;
                    }


                    bool MQTTPacket::is_complete()
                    {
                        return wanted_amount == 0;
                    }


                    bool MQTTPacket::is_error()
                    {
                        return error;
                    }


                    int MQTTPacket::get_send_length()
                    {
                        return static_cast<int>( packet.size());
                    }

                    const uint8_t* MQTTPacket::get_data()
                    {
                        return &packet[0];
                    }

                    const std::vector<uint8_t>& MQTTPacket::get_data_as_vector() const
                    {
                        return packet;
                    }

                    bool MQTTPacket::is_too_big() const
                    {
                        return too_big;
                    }

                    void MQTTPacket::visit(IPacketReceiver& receiver)
                    {
                        receiver.receive(*this);
                    }

                }
            }
        }
    }
}
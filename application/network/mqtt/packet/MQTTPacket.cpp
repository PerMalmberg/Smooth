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

                    int MQTTPacket::get_wanted_amount()
                    {
                        // If we're just reading data to get rid of a too large packet
                        // then we read only one byte at a time.
                        return state == TOO_BIG ? 1 : wanted_amount;
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
                            ByteSet b(packet[bytes_received - 1]);
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
                                    state = TOO_BIG;
                                    too_big = true;
                                    // Make this packet unwanted by the application by setting the type to Reserved.
                                    packet[0] = PacketType::Reserved;
                                }
                                else if (wanted_amount > 0)
                                {
                                    state = DATA;
                                }
                            }
                        }
                        else if (state == DATA || state == TOO_BIG)
                        {
                            wanted_amount -= length;
                        }
                        else
                        {
                            error = true;
                        }
                    }

                    int MQTTPacket::calculate_remaining_length_and_variable_header_offset()
                    {
                        int res = 0;

                        bool done = false;
                        int multiplier = 1;

                        // If present, variable header offset always is at position 2 or later, i.e. the third byte.
                        calculated_variable_header_offset = 2;

                        // The number of length bytes can be up to four
                        for (int i = 0; i < 4 && !done && !error; ++i)
                        {
                            int ix = REMAINING_LENGTH_OFFSET + i;
                            if (ix >= packet.size())
                            {
                                // Outside of vector
                                error = true;
                            }
                            else
                            {
                                ++calculated_variable_header_offset;

                                uint8_t curr = packet[REMAINING_LENGTH_OFFSET + i];

                                res += (curr & 0x7F) * multiplier;
                                multiplier *= 128;
                                ByteSet b(curr);
                                done = !b.test(7);

                                // If we've calculated four items and still not marked as done the
                                // remaining length is malformed.
                                error = !done && i == 3;
                            }
                        }

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

                        ESP_LOGD("encode", "%x", packet[packet.size() - 1]);
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

                    void MQTTPacket::apply_variable_header(const std::vector<uint8_t>& variable)
                    {
                        encode_remaining_length(variable.size());
                        std::copy(variable.begin(), variable.end(), std::back_inserter(packet));
                    }

                    void MQTTPacket::set_header(PacketType type, uint8_t flags)
                    {
                        uint8_t value = (type << 4) | (flags & 0x0F);
                        packet.push_back(value);
                    }

                    std::string MQTTPacket::get_string(int offset) const
                    {
                        uint16_t length = (packet[offset] << 8) | packet[offset+1];
                        std::stringstream ss;
                        for (int i = 0; i < length; ++i)
                        {
                            ss << packet[offset + 2 + i];
                        }
                        return ss.str();
                    }

                    void MQTTPacket::dump() const
                    {
                        std::stringstream ss;

                        ss << "Type: " << get_mqtt_type() << " Length: " << packet.size() << " - ";

                        for (auto b : packet)
                        {
                            ss << std::hex << static_cast<int>( b ) << " ";
                        }

                        ESP_LOGD("mqtt_packet", "%s", ss.str().c_str());
                    }

                    uint8_t* MQTTPacket::get_write_pos()
                    {
                        uint8_t* pos;

                        auto required_size =
                                state == TOO_BIG ? received_header_length + 1 : bytes_received + get_wanted_amount();

                        // Make sure there is room to do direct memory writes by expanding the vector.
                        while (packet.size() < required_size)
                        {
                            packet.push_back(0);
                        }

                        if (state == TOO_BIG)
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
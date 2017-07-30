//
// Created by permal on 7/15/17.
//

#include <bitset>
#include <sstream>
#include <smooth/application/network/mqtt/MQTTPacket.h>
#include "esp_log.h"

using namespace std;

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                MQTTPacket::MQTTPacket(const std::vector<uint8_t> data)
                {
                    std::copy(data.begin(), data.end(), std::back_inserter(packet));
                }

                PacketType MQTTPacket::mqtt_get_type() const
                {
                    return static_cast<PacketType>((packet[0] & 0xFF) >> 4);
                }

                int MQTTPacket::get_wanted_amount()
                {
                    return wanted_amount;
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
                            // We can now calculate the length of the remaining data
                            wanted_amount = calculate_remaining_length();
                            if (wanted_amount > 0)
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

                int MQTTPacket::calculate_remaining_length()
                {
                    int res = 0;

                    bool done = false;
                    int multiplier = 1;

                    // The number of length bytes can be up to four
                    for (int i = 0; i < 4 && !done && !error; ++i)
                    {
                        uint8_t curr = packet[REMAINING_LENGTH_OFFSET + i];

                        res += (curr & 0x7F) * multiplier;
                        multiplier *= 128;
                        ByteSet b(curr);
                        done = !b.test(7);

                        // If we've calculated four items and still not marked as done the
                        // remaining length is malformed.
                        error = !done && i == 3;
                    }

                    if (error)
                    {
                        ESP_LOGE("mqtt", "Invalid remaining length");
                    }
                    else
                    {
                        ESP_LOGD("mqtt", "calculated length: %d", res);
                    }

                    return res;
                }

                void MQTTPacket::encode_remaining_length(int length)
                {
                    for (int i = 0; i < 4 && length > 0; ++i)
                    {
                        packet.push_back(length % 0x80);
                        length /= 0x80;

                        if (length > 0)
                        {
                            packet[packet.size() - 1] |= 0x80;
                        }

                        ESP_LOGD("encode", "%x", packet[packet.size() - 1]);
                    }
                }

                void MQTTPacket::append_string(const std::string& str, std::vector<uint8_t>& target)
                {
                    // Maximum length is 65535 since that is what what can be represented as a 16-bit number.
                    uint16_t length = static_cast<uint16_t>( str.length());
                    target.push_back(length >> 8);
                    target.push_back(length & 0xFF);

                    for (uint16_t i = 0; i < length; ++i)
                    {
                        target.push_back(str[i]);
                    }
                }

                void MQTTPacket::set_header(PacketType type, int flags)
                {
                    uint8_t value = (type << 4) | (flags & 0x0F);
                    packet.push_back(value);
                }

                void MQTTPacket::dump() const
                {
                    std::stringstream ss;

                    ss << "Length: " << packet.size() << " - ";

                    for (auto b : packet)
                    {
                        ss << std::hex << static_cast<int>( b ) << " ";
                    }

                    ESP_LOGD("mqtt_packet", "%s", ss.str().c_str());
                }


                uint8_t* MQTTPacket::get_write_pos()
                {
                    // Make sure there is room to write
                    while (packet.size() < bytes_received + get_wanted_amount())
                    {
                        packet.push_back(0);
                    }

                    return &packet[bytes_received];
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

            }
        }
    }
}
//
// Created by permal on 7/15/17.
//

#include <bitset>
#include <sstream>
#include <smooth/application/network/mqtt/MQTTPacket.h>
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
                            wanted_amount = calculate_remaining_length();

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
                    // Maximum length is 65535 since that is what can be represented as a 16-bit number.
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

                bool MQTTPacket::is_too_big() const
                {
                    return too_big;
                }

            }
        }
    }
}
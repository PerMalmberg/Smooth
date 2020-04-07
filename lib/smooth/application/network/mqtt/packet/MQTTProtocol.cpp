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

#include <bitset>
#include <algorithm>
#include "smooth/application/network/mqtt/packet/MQTTProtocol.h"
#include "smooth/application/network/mqtt/Logging.h"
#include "smooth/core/logging/log.h"
#include "smooth/application/network/mqtt/packet/IPacketReceiver.h"
#include "smooth/config_constants.h"

#ifdef ESP_PLATFORM

#include "sdkconfig.h"

#endif // END ESP_PLATFORM

using namespace smooth::core::logging;
using namespace std;

namespace smooth::application::network::mqtt::packet
{
    int MQTTProtocol::get_wanted_amount(MQTTProtocol::packet_type&)
    {
        // If we're just reading data to get rid of a too large packet
        // then we read the maximum allowed amount, but it will just be overwritten
        // by the next time we read data.
        int wanted;

        if (is_too_big())
        {
            wanted = std::min(remaining_bytes_to_read, CONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE);
        }
        else
        {
            wanted = remaining_bytes_to_read;
        }

        return wanted;
    }

    void MQTTProtocol::data_received(MQTTProtocol::packet_type& packet, int length)
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
            core::util::ByteSet b(packet.data[static_cast<size_t>(bytes_received - 1)]);

            if (b.test(7))
            {
                // Not yet received all length bytes
                remaining_bytes_to_read = 1;
            }
            else
            {
                received_header_length = bytes_received;

                // We can now calculate the length of the remaining data
                remaining_bytes_to_read = packet
                                          .calculate_remaining_length_and_variable_header_offset();

                if (remaining_bytes_to_read > CONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE)
                {
                    Log::verbose(mqtt_log_tag, "Too big packet detected: {} > {}",
                                                    remaining_bytes_to_read,
                                                    CONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE);
                    state = DATA;
                    packet.too_big = true;
                }
                else if (remaining_bytes_to_read > 0)
                {
                    state = DATA;
                }
            }
        }
        else if (state == DATA)
        {
            remaining_bytes_to_read -= length;
        }
        else
        {
            packet.error = true;
        }
    }

    uint8_t* MQTTProtocol::get_write_pos(MQTTProtocol::packet_type& packet)
    {
        uint8_t* pos;

        // Make room for remaining data. In the case of a too big package, allocate
        // maximum allowed to enable quick read of the data.
        auto required_size =
            is_too_big() ?
            received_header_length + CONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE :
            bytes_received + get_wanted_amount(packet);

        // Make sure there is room to do direct memory writes by reserving space.
        packet.data.resize(static_cast<size_t>(required_size), 0);

        if (is_too_big())
        {
            // Always write to the byte after the header
            pos = &packet.data[static_cast<size_t>(received_header_length)];
        }
        else
        {
            // Append to the already received data.
            pos = &packet.data[static_cast<size_t>(bytes_received)];
        }

        return pos;
    }

    bool MQTTProtocol::is_complete(MQTTPacket&) const
    {
        return remaining_bytes_to_read == 0;
    }

    bool MQTTProtocol::is_error()
    {
        return error;
    }

    bool MQTTProtocol::is_too_big() const
    {
        return too_big;
    }

    void MQTTProtocol::packet_consumed()
    {
        state = START;
        error = false;
        too_big = false;
        remaining_bytes_to_read = 1;
        bytes_received = 0;
    }

    void MQTTProtocol::reset()
    {
        packet_consumed();
    }
}

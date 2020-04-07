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

#include <limits>
#include "smooth/application/network/mqtt/packet/Connect.h"
#include "smooth/application/network/mqtt/packet/IPacketReceiver.h"
#include "smooth/application/network/mqtt/MQTTProtocolDefinitions.h"
#include "smooth/core/util/ByteSet.h"

using namespace std;

namespace smooth::application::network::mqtt::packet
{
    // Minimum connect packet
    Connect::Connect(const std::string& client_id, std::chrono::seconds keep_alive, bool clean_session)
            : MQTTPacket(), clean_session(clean_session)
    {
        set_header(PacketType::CONNECT, 0);

        std::vector<uint8_t> variable_header{};

        // Set variable header - Protocol Name
        append_string("MQTT", variable_header);

        // Protocol Level
        variable_header.push_back(4); // v3.1.1

        // Connect Flags
        variable_header.push_back(0);
        core::util::ByteSet connect_flags(0);
        connect_flags.set(0, false); // Reserved
        connect_flags.set(1, clean_session);  // Clean session
        connect_flags.set(2, false); // Will flag
        connect_flags.set(3, false); // Will QoS LSB
        connect_flags.set(4, false); // Will QoS MSB
        connect_flags.set(5, false); // Will retain
        connect_flags.set(6, false); // Password
        connect_flags.set(7, false); // User name
        variable_header[variable_header.size() - 1] = connect_flags;

        // Keep Alive - limit range to 0 ~ std::numeric_limits<uint16_t>::max()
        auto keep_alive_interval = std::max(static_cast<uint16_t>(0),
                                            std::min(std::numeric_limits<uint16_t>::max(),
                                                     static_cast<uint16_t>(keep_alive.count())));

        variable_header.push_back(static_cast<uint8_t>(keep_alive_interval >> 8)); // MSB
        variable_header.push_back(static_cast<uint8_t>(keep_alive_interval & 0xFF)); // LSB

        // Payload, in order if used
        // Client Identifier
        append_string(client_id, variable_header);

        // Will Topic
        // Will Message
        // User Name
        // Password

        apply_constructed_data(variable_header);
    }

    bool Connect::get_clean_session()
    {
        return clean_session;
    }
}

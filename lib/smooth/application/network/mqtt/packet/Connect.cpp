// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <limits>
#include <smooth/application/network/mqtt/packet/Connect.h>
#include <smooth/application/network/mqtt/packet/IPacketReceiver.h>
#include <smooth/application/network/mqtt/MQTTProtocolDefinitions.h>
#include <smooth/core/util/ByteSet.h>

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

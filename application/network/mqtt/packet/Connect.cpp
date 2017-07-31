//
// Created by permal on 7/22/17.
//

#include <limits>
#include <smooth/application/network/mqtt/packet/Connect.h>
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
                namespace packet
                {
                    // Minimum connect packet
                    Connect::Connect(const std::string& client_id, std::chrono::seconds keep_alive)
                            : MQTTPacket()
                    {
                        set_header(PacketType::CONNECT, 0);

                        // Set variable header - Protocol Name
                        append_string("MQTT", variable_header);

                        // Protocol Level
                        variable_header.push_back(4); // v3.1.1

                        // Connect Flags
                        variable_header.push_back(0);
                        ByteSet connect_flags(variable_header[variable_header.size() - 1]);
                        connect_flags.set(0, false); // Reserved
                        connect_flags.set(1, true);  // Clean session
                        connect_flags.set(2, false); // Will flag
                        connect_flags.set(3, false); // Will QoS LSB
                        connect_flags.set(4, false); // Will QoS MSB
                        connect_flags.set(5, false); // Will retain
                        connect_flags.set(6, false); // Password
                        connect_flags.set(7, false); // User name

                        // Keep Alive - limit range to 0 ~ std::numeric_limits<uint16_t>::max()
                        auto keep_alive_interval = std::max(static_cast<uint16_t>(0),
                                                            std::min(std::numeric_limits<uint16_t>::max(),
                                                                     static_cast<uint16_t>( keep_alive.count())));

                        variable_header.push_back(keep_alive_interval >> 8); // MSB
                        variable_header.push_back(keep_alive_interval & 0xFF); // LSB

                        // Payload, in order if used
                        // Client Identifier
                        append_string(client_id, variable_header);

                        // Will Topic
                        // Will Message
                        // User Name
                        // Password

                        encode_remaining_length(variable_header.size());
                        std::copy(variable_header.begin(), variable_header.end(), std::back_inserter(packet));
                    }
                }
            }
        }
    }
}
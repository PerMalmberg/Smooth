//
// Created by permal on 7/22/17.
//

#pragma once

#include <string>
#include <algorithm>
#include <smooth/application/network/mqtt/packet/MQTTPacket.h>

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
                    class Publish
                            : public MQTTPacket
                    {
                        public:
                            Publish() = default;

                            Publish(const MQTTPacket& packet) : MQTTPacket(packet)
                            {
                            }

                            Publish(const std::string& topic, const uint8_t* data, int length, QoS qos, bool retain);

                            void visit(IPacketReceiver& receiver) override;

                            uint16_t get_packet_identifier() const override
                            {
                                uint16_t id = 0;
                                if (has_packet_identifier())
                                {
                                    calculate_remaining_length_and_variable_header_offset();
                                    auto pos = get_variable_header_start() + get_variable_header_length() - 2;
                                    id = read_packet_identifier(pos);
                                }
                                return id;
                            }

                            std::string get_topic() const;

                            std::vector<uint8_t>::const_iterator get_payload_cbegin() const override
                            {
                                return get_variable_header_start() + get_variable_header_length();
                            }

                            std::vector<uint8_t>::const_iterator get_payload_cend() const
                            {
                                return packet.cend();
                            }

                        protected:

                            bool has_packet_identifier() const override
                            {
                                return get_qos() > QoS::AT_MOST_ONCE;
                            }

                            bool has_payload() const override
                            {
                                // Payload is optional for Publish
                                return get_payload_length() > 0;
                            }

                            int get_variable_header_length() const override;
                    };
                }
            }
        }
    }
}

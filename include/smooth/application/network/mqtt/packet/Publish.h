//
// Created by permal on 7/22/17.
//

#pragma once

#include <smooth/application/network/mqtt/packet/MQTTPacket.h>
#include <string>

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

                            QoS get_qos() const
                            {
                                ByteSet b(packet[0]);
                                uint8_t value = b.test(1) | (b.test(2) << 1);
                                return static_cast<QoS>( value );
                            }

                            bool has_packet_identifier() const override
                            {
                                return get_qos() > QoS::AT_MOST_ONCE;
                            }

                            uint16_t get_packet_identifier() override
                            {
                                uint16_t id = 0;

                                if(has_packet_identifier())
                                {
                                    calculate_remaining_length_and_variable_header_offset();
                                    auto index = calculated_variable_header_offset + get_variable_header_length()-2;
                                    id =  packet[index] << 8 | packet[index+1];
                                }


                                return id;
                            }

                            std::string get_topic();
                            uint8_t* get_payload();
                            int get_payload_length();
                        private:

                            int get_variable_header_length()
                            {
                                // Variable header can be calculated based on length of topic + length of optional packet identifier.
                                return get_topic().length()
                                       // Add two for length bytes for string
                                       + 2
                                       + has_packet_identifier() ? 2 : 0;
                            }
                    };
                }
            }
        }
    }
}

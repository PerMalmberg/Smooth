//
// Created by permal on 7/22/17.
//

#pragma once

#include <smooth/application/network/mqtt/packet/MQTTProtocol.h>
#include <smooth/application/network/mqtt/packet/PacketIdentifierFactory.h>

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
                    class Subscribe
                            : public MQTTProtocol
                    {
                        public:
                            Subscribe() = default;

                            explicit Subscribe(const MQTTProtocol& packet) : MQTTProtocol(packet)
                            {
                            }

                            Subscribe(const std::string& topic, QoS qos)
                            {
                                set_header(SUBSCRIBE, 0x2);
                                std::vector<uint8_t> data;
                                append_msb_lsb(PacketIdentifierFactory::get_id(), data);
                                append_string(topic, data);
                                data.push_back(qos);
                                apply_constructed_data(data);
                            }

                            uint16_t get_packet_identifier() const override
                            {
                                return read_packet_identifier(get_variable_header_start());
                            }

                            void get_topics(std::vector<std::pair<std::string, QoS>>& topics) const;

                            void visit(IPacketReceiver& receiver) override;
                        protected:
                            int get_variable_header_length() const override
                            {
                                return 2;
                            }

                            bool has_packet_identifier() const override
                            {
                                return true;
                            }

                            bool has_payload() const override
                            {
                                return true;
                            }
                    };
                }
            }
        }
    }
}

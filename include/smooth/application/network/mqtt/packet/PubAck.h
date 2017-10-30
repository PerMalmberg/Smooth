//
// Created by permal on 7/22/17.
//

#pragma once

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
                    class PubAck
                            : public MQTTPacket
                    {
                        public:
                            PubAck() = default;

                            PubAck(uint16_t packet_id)
                            {
                                set_header(PUBACK, 0x2);
                                std::vector<uint8_t> variable_header;
                                append_msb_lsb(packet_id, variable_header);
                                apply_constructed_data(variable_header);
                            }

                            PubAck(const MQTTPacket& packet) : MQTTPacket(packet)
                            {
                            }

                            void visit( IPacketReceiver& receiver ) override;

                            uint16_t get_packet_identifier() const override
                            {
                                return read_packet_identifier(get_variable_header_start());
                            }
                        protected:

                            bool has_packet_identifier() const override
                            {
                                return true;
                            }
                    };
                }
            }
        }
    }
}

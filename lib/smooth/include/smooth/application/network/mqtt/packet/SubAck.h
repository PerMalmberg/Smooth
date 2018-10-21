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
                    class SubAck
                            : public MQTTPacket
                    {
                        public:
                            SubAck() = default;

                            explicit SubAck(const MQTTPacket& packet) : MQTTPacket(packet)
                            {
                            }

                            void visit(IPacketReceiver& receiver) override;

                            uint16_t get_packet_identifier() const override
                            {
                                return read_packet_identifier(get_variable_header_start());
                            }

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

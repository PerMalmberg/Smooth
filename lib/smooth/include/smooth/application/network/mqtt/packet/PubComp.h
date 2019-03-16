//
// Created by permal on 7/22/17.
//

#pragma once

#include <smooth/application/network/mqtt/packet/MQTTProtocol.h>

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
                    class PubComp
                            : public MQTTProtocol
                    {
                        public:
                            PubComp() = default;

                            explicit PubComp(uint16_t packet_id)
                            {
                                set_header(PUBCOMP, 0x2);
                                std::vector<uint8_t> variable_header;
                                append_msb_lsb(packet_id, variable_header);
                                apply_constructed_data(variable_header);
                            }

                            explicit PubComp(const MQTTProtocol& packet) : MQTTProtocol(packet)
                            {
                            }

                            void visit(IPacketReceiver& receiver) override;

                            uint16_t get_packet_identifier() const override;

                        protected:
                            bool has_packet_identifier() const override
                            {
                                return true;
                            }

                            int get_variable_header_length() const override
                            {
                                // Only packet identifier in this variable header
                                return 2;
                            }
                    };
                }
            }
        }
    }
}

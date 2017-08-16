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
                    class PubRec
                            : public MQTTPacket
                    {
                        public:
                            PubRec() = default;

                            PubRec(uint16_t packet_id)
                            {
                                set_header(PUBREC, 0);
                                std::vector<uint8_t> variable_header;
                                append_msb_lsb(packet_id, variable_header);
                                apply_constructed_data(variable_header);
                            }

                            PubRec(const MQTTPacket& packet) : MQTTPacket(packet)
                            {
                            }

                            void visit( IPacketReceiver& receiver ) override;

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

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
                    class PubRel
                            : public MQTTPacket
                    {
                        public:
                            PubRel() = default;

                            explicit PubRel(const MQTTPacket& packet) : MQTTPacket(packet)
                            {
                            }

                            explicit PubRel(uint16_t packet_id)
                            {
                                // Set fixed header
                                set_header(PUBREL, 0x2);

                                std::vector<uint8_t> variable_header;
                                append_msb_lsb(packet_id, variable_header);
                                apply_constructed_data(variable_header);
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

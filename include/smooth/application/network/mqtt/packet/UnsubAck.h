//
// Created by permal on 7/22/17.
//

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
                    class UnsubAck
                            : public MQTTPacket
                    {
                        public:
                            UnsubAck() = default;

                            UnsubAck(const MQTTPacket& packet) : MQTTPacket(packet)
                            {
                            }

                            virtual uint16_t get_packet_identifier() const
                            {
                                return read_packet_identifier(variable_header_start);
                            }

                            void visit(IPacketReceiver& receiver) override;
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

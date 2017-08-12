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
                    class Unsubscribe
                            : public MQTTPacket
                    {
                        public:
                            Unsubscribe() = default;

                            Unsubscribe(const MQTTPacket& packet) : MQTTPacket(packet)
                            {
                            }

                            void visit(IPacketReceiver& receiver) override;

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

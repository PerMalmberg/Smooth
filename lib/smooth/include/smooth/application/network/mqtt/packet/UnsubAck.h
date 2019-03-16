//
// Created by permal on 7/22/17.
//

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
                    class UnsubAck
                            : public MQTTProtocol
                    {
                        public:
                            UnsubAck() = default;

                            explicit UnsubAck(const MQTTProtocol& packet) : MQTTProtocol(packet)
                            {
                            }

                            uint16_t get_packet_identifier() const override
                            {
                                return read_packet_identifier(get_variable_header_start());
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

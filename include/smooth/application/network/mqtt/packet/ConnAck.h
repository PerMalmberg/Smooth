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
                    class ConnAck
                            : public MQTTPacket
                    {
                        public:
                            enum ReturnCode
                            {
                                ACCEPTED = 0,
                                UNSUPPORTED_PROTOCOL_VERSION,
                                REJECTED_IDENTIFIER,
                                SERVER_UNAVAILABLE,
                                BAD_CREDENTIALS,
                                NOT_AUTHORIZED,
                            };

                            ConnAck() = default;

                            ConnAck(const MQTTPacket& packet) : MQTTPacket(packet)
                            {
                            }

                            bool is_session_present()
                            {
                                ByteSet b(packet[VARIABLE_HEADER_OFFSET]);
                                return b.test(0);
                            }

                            ReturnCode get_return_code()
                            {
                                return static_cast<ReturnCode>(packet[VARIABLE_HEADER_OFFSET+1]);
                            }

                            bool was_connection_accepted()
                            {
                                return get_return_code() == ACCEPTED;
                            }

                            void visit( IPacketReceiver& receiver ) override;
                    };
                }
            }
        }
    }
}

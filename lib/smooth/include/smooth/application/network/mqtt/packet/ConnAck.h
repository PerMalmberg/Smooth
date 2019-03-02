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

                            explicit ConnAck(const MQTTPacket& other_packet) : MQTTPacket(other_packet)
                            {
                            }

                            bool is_session_present()
                            {
                                core::util::ByteSet b(*get_variable_header_start());
                                return b.test(0);
                            }

                            ReturnCode get_return_code()
                            {
                                return static_cast<ReturnCode>(*(get_variable_header_start()+1));
                            }

                            bool connection_was_accepted()
                            {
                                return get_return_code() == ACCEPTED;
                            }

                            void visit( IPacketReceiver& receiver ) override;
                        protected:
                            uint32_t get_variable_header_length() const override
                            {
                                return 2;
                            }
                    };
                }
            }
        }
    }
}

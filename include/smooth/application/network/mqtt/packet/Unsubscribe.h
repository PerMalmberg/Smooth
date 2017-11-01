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

                            Unsubscribe(const std::string& topic)
                            {
                                set_header(UNSUBSCRIBE, 0x2);
                                std::vector<uint8_t> data;
                                append_msb_lsb(PacketIdentifierFactory::get_id(), data);
                                append_string(topic, data);
                                apply_constructed_data(data);
                            }


                            Unsubscribe(const MQTTPacket& packet) : MQTTPacket(packet)
                            {
                            }

                            uint16_t get_packet_identifier() const override
                            {
                                return read_packet_identifier(get_variable_header_start());
                            }

                            void get_topics(std::vector<std::string>& topics) const;

                            void visit(IPacketReceiver& receiver) override;
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

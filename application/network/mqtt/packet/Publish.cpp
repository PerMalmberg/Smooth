//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/Publish.h>

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
                    Publish::Publish(const std::string& topic, const uint8_t* data, int length, QoS qos, bool retain)
                    {
                        core::util::ByteSet flags(0);
                        flags.set(0, retain);
                        flags.set(1, qos & 0x1);
                        flags.set(2, qos & 0x2);
                        flags.set(3, get_send_retry_count() > 0);
                        set_header(PacketType::PUBLISH, flags);

                        std::vector<uint8_t> variable_header{};
                        // Topic
                        append_string(topic, variable_header);

                        // Packet identifier
                        if (has_packet_identifier())
                        {
                            append_msb_lsb(PacketIdentifierFactory::get_id(), variable_header);
                        }

                        // Payload
                        append_data(data, length, variable_header);

                        apply_variable_header(variable_header);
                    }

                    std::string Publish::get_topic() const
                    {
                        calculate_remaining_length_and_variable_header_offset();
                        return get_string(variable_header_start);
                    }

                    void Publish::visit(IPacketReceiver& receiver)
                    {
                        receiver.receive(*this);
                    }
                }
            }
        }
    }
}

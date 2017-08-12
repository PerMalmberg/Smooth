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
                        uint8_t flags = 0;
                        ByteSet f(flags);
                        f.set(0, retain);
                        f.set(1, qos & 0x1);
                        f.set(2, qos & 0x2);
                        f.set(3, get_send_retry_count() > 0);
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

                    std::string Publish::get_topic()
                    {
                        calculate_remaining_length_and_variable_header_offset();
                        return get_string(calculated_variable_header_offset);
                    }

                    uint8_t* Publish::get_payload()
                    {
                        calculate_remaining_length_and_variable_header_offset();
                        return &packet[calculated_variable_header_offset+get_variable_header_length()];
                    }

                    int Publish::get_payload_length()
                    {
                        // Length of payload is (length of remaining bytes in fixed header) minus (length of variable header)
                        int payload_length = calculate_remaining_length_and_variable_header_offset() - get_variable_header_length();

                        return payload_length;
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

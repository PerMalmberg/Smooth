//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/Subscribe.h>
#include <smooth/core/util/advance_iterator.h>
#include <smooth/application/network/mqtt/packet/IPacketReceiver.h>

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
                    void Subscribe::visit(IPacketReceiver& receiver)
                    {
                        receiver.receive(*this);
                    }

                    void Subscribe::get_topics(std::vector<std::pair<std::string, QoS>>& topics) const
                    {
                        calculate_remaining_length_and_variable_header_offset();
                        auto it = get_variable_header_start() + get_variable_header_length();
                        while (it != data.end())
                        {
                            auto s = get_string(it);
                            // Move to QoS after string, add two for the length bits
                            if (core::util::advance(it, data.end(), s.length() + 2))
                            {
                                auto qos = static_cast<QoS>(*it);
                                topics.emplace_back(s, qos);
                                // Move past QoS to next string
                                it++;
                            }
                            else
                            {
                                // Uho, packet is malformed, skip rest.
                                it = data.end();
                            }
                        }
                    }
                }
            }
        }
    }
}

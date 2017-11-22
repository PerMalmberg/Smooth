//
// Created by permal on 7/22/17.
//

#include <smooth/application/network/mqtt/packet/Unsubscribe.h>
#include <smooth/core/util/advance_iterator.h>

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
                    void Unsubscribe::visit(IPacketReceiver& receiver)
                    {
                        receiver.receive(*this);
                    }

                    void Unsubscribe::get_topics(std::vector<std::string>& topics) const
                    {
                        calculate_remaining_length_and_variable_header_offset();
                        auto it = get_variable_header_start() + get_variable_header_length();

                        bool end_reached = false;

                        do
                        {
                            auto s = get_string(it);
                            topics.push_back(s);
                            // Move to next string, add two for the length bits
                            end_reached = !core::util::advance(it, packet.end(), s.length() + 2);
                        }
                        while (!end_reached && it != packet.end());
                    }
                }
            }
        }
    }
}

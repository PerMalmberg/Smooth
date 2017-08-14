//
// Created by permal on 8/14/17.
//

#include <smooth/application/network/mqtt/Subscription.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                void Subscription::subscribe(const std::string& topic, QoS qos)
                {
                    auto found = sub.find(topic);
                    if (found == sub.end())
                    {
                        auto& info = (*found).second;
                        if (info.get_qos() != qos)
                        {
                            // Replace with new info
                            info = SubscriptionInfo(qos);
                        }
                    }
                    else
                    {
                        // New subscription
                        sub.insert(std::make_pair(topic, SubscriptionInfo(qos)));
                    }
                }

                void Subscription::receive(packet::Publish& publish, IMqtt& mqtt)
                {
                    // Send to application

                }

                void Subscription::receive(packet::SubAck& sub_ack, IMqtt& mqtt)
                {
                }

                void Subscription::receive(packet::UnsubAck& unsub_ack, IMqtt& mqtt)
                {
                }

                void Subscription::receive(packet::PubRel& pub_rel, IMqtt& mqtt)
                {
                }
            }
        }
    }
}


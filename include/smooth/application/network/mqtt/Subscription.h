//
// Created by permal on 8/14/17.
//

#pragma once

#include <unordered_map>
#include <smooth/application/network/mqtt/SubscriptionInfo.h>
#include <smooth/application/network/mqtt/packet/PubAck.h>
#include <smooth/application/network/mqtt/packet/PubComp.h>
#include <smooth/application/network/mqtt/packet/Publish.h>
#include <smooth/application/network/mqtt/packet/PubRec.h>
#include <smooth/application/network/mqtt/packet/PubRel.h>
#include <smooth/application/network/mqtt/packet/SubAck.h>
#include <smooth/application/network/mqtt/packet/Subscribe.h>
#include <smooth/application/network/mqtt/packet/UnsubAck.h>
#include <smooth/application/network/mqtt/packet/Unsubscribe.h>
#include <smooth/application/network/mqtt/IMqtt.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                class Subscription
                {
                    public:
                        void subscribe(const std::string& topic, QoS qos);

                        void receive(packet::Publish& publish, IMqtt& mqtt);
                        void receive(packet::SubAck& sub_ack, IMqtt& mqtt);
                        void receive(packet::UnsubAck& unsub_ack, IMqtt& mqtt);
                        void receive(packet::PubRel& pub_rel, IMqtt& mqtt);
                    private:
                        std::unordered_map<std::string, SubscriptionInfo> sub{};
                };
            }
        }
    }
}


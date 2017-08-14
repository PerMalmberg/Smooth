//
// Created by permal on 8/14/17.
//

#pragma once

#include <smooth/application/network/mqtt/MQTTProtocolDefinitions.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                class SubscriptionInfo
                {
                    public:
                        SubscriptionInfo() = default;
                        SubscriptionInfo(const SubscriptionInfo&) = default;
                        SubscriptionInfo& operator=(const SubscriptionInfo&) = default;

                        SubscriptionInfo(QoS qos)
                                : qos(qos)
                        {
                        }

                        QoS get_qos() const
                        {
                            return qos;
                        }

                        bool is_subscribed() const
                        {
                            return subscribed;
                        }

                        void set_subscribed(bool status)
                        {
                            subscribed = status;
                        }

                    private:
                        QoS qos;
                        bool subscribed = false;
                };
            }
        }
    }
}

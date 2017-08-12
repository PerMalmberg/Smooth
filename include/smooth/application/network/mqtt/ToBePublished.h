//
// Created by permal on 8/12/17.
//

#pragma once

#include <vector>
#include <smooth/application/network/mqtt/packet/PubAck.h>
#include <smooth/application/network/mqtt/packet/PubComp.h>
#include <smooth/application/network/mqtt/packet/Publish.h>
#include <smooth/application/network/mqtt/packet/PubRec.h>
#include <smooth/application/network/mqtt/IMqtt.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                class ToBePublished
                {
                    public:
                        void
                        publish(const std::string& topic, const uint8_t* data, int length, mqtt::QoS qos, bool retain);

                        void publish_next(IMqtt& mqtt);

                    private:

                        class InFlight
                        {
                            public:
                                InFlight(packet::Publish& p)
                                        : p(p)
                                {
                                }

                                packet::Publish& get_packet()
                                {
                                    return p;
                                }

                                int get_step()
                                {
                                    return publish_steps;
                                }

                                void increase_step()
                                {
                                    ++publish_steps;
                                }

                            private:
                                packet::Publish p{};
                                int publish_steps;
                        };

                        std::vector<InFlight> in_progress{};
                };
            }
        }
    }
}
//
// Created by permal on 8/12/17.
//

#pragma once

#include <vector>
#include <chrono>
#include <smooth/core/timer/PerfCount.h>
#include <smooth/application/network/mqtt/packet/PubAck.h>
#include <smooth/application/network/mqtt/packet/PubComp.h>
#include <smooth/application/network/mqtt/packet/Publish.h>
#include <smooth/application/network/mqtt/packet/PubRec.h>
#include <smooth/application/network/mqtt/IMqtt.h>
#include <smooth/application/network/mqtt/InFlight.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                class Publication
                {
                    public:
                        Publication();

                        bool
                        publish(const std::string& topic, const uint8_t* data, int length, mqtt::QoS qos, bool retain);

                        void publish_next(IMqtt& mqtt);

                        void handle_disconnect();

                        void resend_outstanding_control_packet(IMqtt& mqtt);

                        void receive(packet::PubAck& pub_ack, IMqtt& mqtt);

                        void receive(packet::PubRec& pub_rec, IMqtt& mqtt);

                        void receive(packet::PubComp& pub_rel, IMqtt& mqtt);

                    private:
                        std::vector<InFlight<packet::Publish>> in_progress{};
                };
            }
        }
    }
}
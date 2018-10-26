//
// Created by permal on 8/12/17.
//

#pragma once

#include <vector>
#include <chrono>
#include <mutex>
#include <smooth/core/timer/ElapsedTime.h>
#include <smooth/application/network/mqtt/packet/PubAck.h>
#include <smooth/application/network/mqtt/packet/PubComp.h>
#include <smooth/application/network/mqtt/packet/Publish.h>
#include <smooth/application/network/mqtt/packet/PubRec.h>
#include <smooth/application/network/mqtt/IMqttClient.h>
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

                        void publish_next(IMqttClient& mqtt);

                        void handle_disconnect();

                        void resend_outstanding_control_packet(IMqttClient& mqtt, bool clean_session);

                        void receive(packet::PubAck& pub_ack, IMqttClient& mqtt);

                        void receive(packet::PubRec& pub_rec, IMqttClient& mqtt);

                        void receive(packet::PubComp& pub_rel, IMqttClient& mqtt);

                    private:
                        std::vector<InFlight<packet::Publish>> in_progress{};
                        std::mutex guard{};
                };
            }
        }
    }
}
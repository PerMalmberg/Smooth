/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <vector>
#include <chrono>
#include <mutex>
#include "smooth/core/timer/ElapsedTime.h"
#include "smooth/application/network/mqtt/packet/PubAck.h"
#include "smooth/application/network/mqtt/packet/PubComp.h"
#include "smooth/application/network/mqtt/packet/Publish.h"
#include "smooth/application/network/mqtt/packet/PubRec.h"
#include "smooth/application/network/mqtt/IMqttClient.h"
#include "smooth/application/network/mqtt/InFlight.h"

namespace smooth::application::network::mqtt
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

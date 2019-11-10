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
#include <unordered_map>
#include <mutex>
#include "smooth/application/network/mqtt/packet/PubAck.h"
#include "smooth/application/network/mqtt/packet/PubComp.h"
#include "smooth/application/network/mqtt/packet/Publish.h"
#include "smooth/application/network/mqtt/packet/PubRec.h"
#include "smooth/application/network/mqtt/packet/PubRel.h"
#include "smooth/application/network/mqtt/packet/SubAck.h"
#include "smooth/application/network/mqtt/packet/Subscribe.h"
#include "smooth/application/network/mqtt/packet/UnsubAck.h"
#include "smooth/application/network/mqtt/packet/Unsubscribe.h"
#include "smooth/application/network/mqtt/IMqttClient.h"
#include "smooth/application/network/mqtt/InFlight.h"
#include "smooth/application/network/mqtt/Logging.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::application::network::mqtt
{
    class Subscription
    {
        public:
            void subscribe(const std::string& topic, QoS qos);

            void unsubscribe(const std::string& topic);

            void receive(packet::Publish& publish, IMqttClient& mqtt);

            void receive(packet::SubAck& sub_ack, IMqttClient& mqtt);

            void receive(packet::UnsubAck& unsub_ack, IMqttClient& mqtt);

            void receive(packet::PubRel& pub_rel, IMqttClient& mqtt);

            void subscribe_next(IMqttClient& mqtt);

            void handle_disconnect();

        private:
            void forward_to_application(const packet::Publish& publish, IMqttClient& mqtt) const;

            template<typename T>
            bool send_control_packet(std::vector<InFlight<T>>& in_flight, PacketType wait_for, IMqttClient& mqtt,
                                     const char* control_type)
            {
                bool all_ok = true;

                if (in_flight.size() > 0)
                {
                    auto& flight = in_flight.front();

                    if (flight.get_waiting_for() == PacketType::Reserved)
                    {
                        auto& packet = flight.get_packet();

                        if (mqtt.send_packet(packet))
                        {
                            flight.start_timer();
                            flight.set_wait_packet(wait_for);
                        }
                    }
                    else if (flight.get_elapsed_time() > std::chrono::seconds(5))
                    {
                        // Waited too long, force a disconnect.
                        Log::error(mqtt_log_tag,
                                   "Too long since a reply was received to a {} request, forcing disconnect.",
                                   control_type);
                        flight.stop_timer();
                        all_ok = false;
                        mqtt.force_disconnect();
                    }
                }

                return all_ok;
            }

            template<typename T>
            void reset_control_packet(std::vector<InFlight<T>>& in_flight)
            {
                auto first = in_flight.begin();

                if (first != in_flight.end())
                {
                    auto& flight = *first;
                    flight.set_wait_packet(PacketType::Reserved);
                    flight.stop_timer();
                }
            }

            void internal_subscribe(const std::string& topic, const QoS& qos);

            std::unordered_map<uint16_t, InFlight<packet::Publish>> receiving{};
            std::vector<InFlight<packet::Subscribe>> subscribing{};
            std::unordered_map<std::string, QoS> active_subscription{};
            std::vector<InFlight<packet::Unsubscribe>> unsubscribing{};
            std::mutex guard{};
    };
}

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

#include <chrono>
#include "smooth/core/ipc/TaskEventQueue.h"

namespace smooth::application::network::mqtt
{
    class Publication;

    class Subscription;

    class IMqttClient
    {
        public:
            virtual ~IMqttClient() = default;

            virtual const std::string& get_client_id() const = 0;

            virtual std::chrono::seconds get_keep_alive() const = 0;

            virtual void start_reconnect() = 0;

            virtual void reconnect() = 0;

            virtual bool is_auto_reconnect() const = 0;

            virtual void disconnect() = 0;

            virtual void force_disconnect() = 0;

            virtual void set_keep_alive_timer(std::chrono::seconds interval) = 0;

            virtual bool send_packet(packet::MQTTPacket& packet) = 0;

            virtual Publication& get_publication() = 0;

            virtual Subscription& get_subscription() = 0;

            virtual std::weak_ptr<core::ipc::TaskEventQueue<std::pair<std::string, std::vector<uint8_t>>>>
            get_application_queue() = 0;
    };
}

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

#include "smooth/core/Application.h"
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/application/network/mqtt/MqttClient.h"
#include <random>

#ifdef ESP_PLATFORM
#include "smooth/core/io/Output.h"
#endif

namespace mqtt
{
    class App
        : public smooth::core::Application,
        smooth::core::ipc::IEventListener<smooth::application::network::mqtt::MQTTData>
    {
        public:
            App();

            void init() override;

            void event(const smooth::application::network::mqtt::MQTTData& event) override;

            void send_message();

            void tick() override;

        private:
            using MQTTDataQueue = smooth::core::ipc::TaskEventQueue<smooth::application::network::mqtt::MQTTData>;
            std::shared_ptr<MQTTDataQueue> mqtt_data;
            smooth::application::network::mqtt::MqttClient client;
            std::random_device rand{};
            std::mt19937 gen{ rand() };
            std::uniform_int_distribution<> dis{ 1, 3 };
    };
}

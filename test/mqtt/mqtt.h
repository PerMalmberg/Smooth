// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <smooth/core/Application.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/application/network/mqtt/MqttClient.h>
#include <random>

#ifdef ESP_PLATFORM
#include <smooth/core/io/Output.h>
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
        std::mt19937 gen{rand()};
        std::uniform_int_distribution<> dis{1, 3};
    };

}
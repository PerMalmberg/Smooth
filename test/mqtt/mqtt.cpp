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

#include "mqtt.h"
#include <smooth/core/task_priorities.h>

#ifdef ESP_PLATFORM
#include <smooth/core/network/Wifi.h>
#include "wifi_creds.h"
#endif

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::logging;
using namespace std::chrono;
using namespace smooth::application::network::mqtt;

namespace mqtt
{
    static const char* broker = "192.168.10.245";
#ifdef ESP_PLATFORM
    static const char* client_id = "ESP32";
#else
    static const char* client_id = "Linux";
#endif

    App::App()
            : Application(APPLICATION_BASE_PRIO, seconds(10)),
              mqtt_data(MQTTDataQueue::create("mqtt_data", 10, *this, *this)),
              client(client_id, seconds(10), 8192, 10, mqtt_data)
    {
    }

    void App::init()
    {
        Application::init();
#ifdef ESP_PLATFORM
        Log::info("App::Init", Format("Starting wifi..."));
        network::Wifi& wifi = get_wifi();
        wifi.set_host_name("Smooth-ESP");
        wifi.set_auto_connect(true);
        wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
        wifi.connect_to_ap();
#endif

        client.connect_to(std::make_shared<smooth::core::network::IPv4>(broker, 1883), true);
        client.subscribe("network_test", QoS::EXACTLY_ONCE);
        client.subscribe("$SYS/broker/uptime", QoS::AT_LEAST_ONCE);
        send_message();
    }

    void App::event(const smooth::application::network::mqtt::MQTTData& event)
    {
        Log::info("Rec", Format("T:{1}, M:{2}", Str(event.first), Vector<uint8_t>(event.second, true)));

        send_message();
    }

    void App::tick()
    {
        client.publish("network_test", "Message", QoS::EXACTLY_ONCE, false);
    }

    void App::send_message()
    {
        static uint32_t len = 0;

        std::string rep(len, 'Q');
        auto v = dis(gen);

        if (v == 1)
        {
            client.publish("network_test", rep, QoS::EXACTLY_ONCE, false);
        }
        else if (v == 2)
        {
            client.publish("network_test", rep, QoS::AT_MOST_ONCE, false);
        }
        else
        {
            client.publish("network_test", rep, QoS::AT_LEAST_ONCE, false);
        }

        if (++len == 3000)
        {
            len = 1;
        }
    }
}

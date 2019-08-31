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

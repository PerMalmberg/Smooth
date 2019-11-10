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

#include "sntp.h"
#include <chrono>
#include <ctime>
#include "smooth/core/Task.h"
#include "smooth/core/task_priorities.h"
#include "smooth/core/network/Wifi.h"
#include "wifi_creds.h"
#include <iostream>

using namespace smooth::core;
using namespace std::chrono;

namespace sntp
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1)),
              sntp(std::vector<std::string>{ "0.se.pool.ntp.org", "1.se.pool.ntp.org" }),
              sync_queue(TimeSyncQueue::create(2, *this, *this))
    {
    }

    void App::init()
    {
#ifdef ESP_PLATFORM
        assert(!sntp.is_time_set());
#endif
        std::cout << "Time at startup:";
        print_time();
        sntp.start();

        Log::info("App::Init", "Starting wifi...");
        network::Wifi& wifi = get_wifi();
        wifi.set_host_name("Smooth-ESP");
        wifi.set_auto_connect(true);
        wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
        wifi.connect_to_ap();
    }

    void App::tick()
    {
        std::cout << "Tick!" << std::endl;
        print_time();
    }

    void App::print_time() const
    {
        auto t = system_clock::to_time_t(system_clock::now());
        tm time{};
        localtime_r(&t, &time);
        std::cout << asctime(&time) << std::endl;
    }

    void App::event(const smooth::core::sntp::TimeSyncEvent& ev)
    {
        auto t = system_clock::to_time_t(ev.get_timePoint());
        tm time{};
        localtime_r(&t, &time);
        std::cout << "Time set at at: " << asctime(&time) << std::endl;
    }
}

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

#include "sntp.h"
#include <chrono>
#include <ctime>
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/network/Wifi.h>
#include "wifi_creds.h"
#include <iostream>

using namespace smooth::core;
using namespace std::chrono;

namespace sntp
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1)),
              sntp(std::vector<std::string>{ "0.se.pool.ntp.org", "1.se.pool.ntp.org" }),
              sync_queue(TimeSyncQueue::create("", 2, *this, *this))
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

        Log::info("App::Init", Format("Starting wifi..."));
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
        std::cout << "Time set at at: " << asctime(&time) << std::endl;;
    }
}

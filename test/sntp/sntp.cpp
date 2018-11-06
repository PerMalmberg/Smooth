//
// Created by permal on 2018-10-21.
//

#include "sntp.h"
#include <chrono>
#include <cassert>
#include <time.h>
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include "wifi_creds.h"

using namespace smooth::core;
using namespace std::chrono;

namespace sntp
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1)),
            sntp(std::vector<std::string>{"0.se.pool.ntp.org", "1.se.pool.ntp.org"})
    {
    }

    void App::init()
    {
        assert(!sntp.is_time_set());
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


}
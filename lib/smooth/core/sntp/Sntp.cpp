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

#include "smooth/core/sntp/Sntp.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <esp_sntp.h>
#pragma GCC diagnostic pop
#include <ctime>
#include <utility>
#include "smooth/core/ipc/Publisher.h"
#include "smooth/core/sntp/TimeSyncEvent.h"

namespace smooth::core::sntp
{
    Sntp::Sntp(std::vector<std::string> servers)
            : servers(std::move(servers))
    {
    }

    void Sntp::start()
    {
        if (!started)
        {
            sntp_setoperatingmode(SNTP_OPMODE_POLL);

            uint8_t i = 0;

            for (auto& s : servers)
            {
                // Only a pointer to the string is stored in the underlying structs.
                sntp_setservername(i++, const_cast<char*>(s.c_str()));
            }

            sntp_set_time_sync_notification_cb(&Sntp::timeSyncNotificationCallback);
            sntp_init();
        }

#ifndef ESP_PLATFORM

        // Simulate sync
        publish_sync_event(std::chrono::system_clock::now());
#endif
    }

    bool Sntp::is_time_set() const
    {
        time_t now{};
        tm timeinfo{};
        time(&now);
        localtime_r(&now, &timeinfo);

        return timeinfo.tm_year > 70;
    }

    void Sntp::timeSyncNotificationCallback(timeval* tv)
    {
        auto dateTime = system_clock::from_time_t(tv->tv_sec) + microseconds(tv->tv_usec);
        publish_sync_event(dateTime);
    }

    void Sntp::publish_sync_event(const std::chrono::system_clock::time_point& tp)
    {
        TimeSyncEvent ev(tp);
        smooth::core::ipc::Publisher<TimeSyncEvent>::publish(ev);
    }
}

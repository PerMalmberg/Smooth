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
#include <ctime>
#include <string>
#include <vector>

namespace smooth::core::sntp
{
    /// Handles SNTP.
    class Sntp
    {
        public:
            /// \arg servers The sntp servers to use.
            explicit Sntp(std::vector<std::string> servers);

            void start();

            [[nodiscard]] bool is_time_set() const;

        private:
            const std::vector<std::string> servers;
            bool started = false;

            static void timeSyncNotificationCallback(struct timeval* tv);

            static void publish_sync_event(const std::chrono::system_clock::time_point& tp);
    };
}

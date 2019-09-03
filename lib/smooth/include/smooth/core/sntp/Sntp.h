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

#include <chrono>
#include <ctime>
#include <string>
#include <vector>

namespace smooth::core::sntp
{
    class Sntp
    {
        public:
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

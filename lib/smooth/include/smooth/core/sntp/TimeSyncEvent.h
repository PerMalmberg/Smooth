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

namespace smooth::core::sntp
{
    using namespace std::chrono;

    /// Event sent when a SNTP sync notification occurs
    class TimeSyncEvent
    {
        public:
            explicit TimeSyncEvent(const system_clock::time_point& timePoint)
                    : timePoint(timePoint)
            {
            }

            TimeSyncEvent() = default;

            TimeSyncEvent(const TimeSyncEvent&) = default;

            TimeSyncEvent& operator=(const TimeSyncEvent&) = default;

            /// Gets the system_clock::time_point of the SNTP sync notification
            /// \return The SNTP sync notification timeval.
            [[nodiscard]] system_clock::time_point get_timePoint() const
            {
                return timePoint;
            }

        private:
            system_clock::time_point timePoint = system_clock::from_time_t(0);
    };
}

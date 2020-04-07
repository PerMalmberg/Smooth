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

#include "smooth/core/timer/ITimer.h"

namespace smooth::core::timer
{
    /// Event sent when a timer expires
    class TimerExpiredEvent
    {
        public:
            TimerExpiredEvent() = default;

            explicit TimerExpiredEvent(int id)
                    : id(id)
            {
            }

            /// Gets the timer that has expired
            /// \return The timer that expired.
            [[nodiscard]] int get_id() const
            {
                return id;
            }

        private:
            int id = -1;
    };
}

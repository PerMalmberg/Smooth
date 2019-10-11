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

#include <string>
#include <chrono>

namespace smooth::core::timer
{
    /// Interface for timers
    class ITimer
    {
        public:
            virtual ~ITimer() = default;

            /// Starts the timer with the already set interval.
            virtual void start() = 0;

            /// Starts the timer with the specified interval.
            /// \param interval The new interval
            virtual void start(std::chrono::milliseconds interval) = 0;

            /// Stops the timer
            virtual void stop() = 0;

            /// Resets the timer, i.e. starts a new interval.
            virtual void reset() = 0;

            /// Gets the ID of the timer.
            /// \return The id.
            [[nodiscard]] virtual int get_id() const = 0;
    };
}

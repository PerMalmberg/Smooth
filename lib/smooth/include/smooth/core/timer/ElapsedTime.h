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

namespace smooth::core::timer
{
    /// Performance/time keeping timer. Used to measure the time between two points in time.
    class ElapsedTime
    {
        public:
            /// Starts the performance timer.
            void start()
            {
                active = true;
                zero();
            }

            /// Stops the performance timer
            void stop()
            {
                end_time = std::chrono::steady_clock::now();
                active = false;
                elapsed = end_time - start_time;
            }

            /// Functionally the same as start(), but provided for syntactical reasons.
            void reset()
            {
                // Simply restart the timer.
                start();
            }

            /// Zeroes the time, but lets it keep running.
            void zero()
            {
                start_time = std::chrono::steady_clock::now();
                end_time = start_time;
            }

            /// Stops the timer and zeroes it.
            void stop_and_zero()
            {
                stop();
                zero();
            }

            /// Gets the amount of time passed since start.
            /// \return The amount of time.
            std::chrono::microseconds get_running_time();

            [[nodiscard]] std::chrono::microseconds get_running_time() const;

            /// \returns true if the timer is running, false if not.
            [[nodiscard]] bool is_running() const
            {
                return active;
            }

        private:
            bool active = false;
            std::chrono::steady_clock::time_point start_time{};
            std::chrono::steady_clock::time_point end_time{};
            std::chrono::steady_clock::duration elapsed{};
    };
}

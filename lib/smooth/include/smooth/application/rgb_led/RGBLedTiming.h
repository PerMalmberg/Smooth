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

#include <driver/rmt.h>

namespace smooth::application::rgb_led
{
    /// Base class for RGB LED timing data.
    /// @note All durations are in nano seconds.
    /// @note Only the first period level is specified, the second one
    /// will be the inverse of the first.
    class RGBLedTiming
    {
        protected:
            /// Constructor
            /// \param low_duration_0_level Level of first part of the period for 'low'
            /// \param low_duration_0 Duration of first period for 'low'
            /// \param low_duration_1 Duration of second period for 'low'
            /// \param high_duration_0_level Level of first part of the period for 'high'
            /// \param high_duration_0 Duration of first period for 'high'
            /// \param high_duration_1 Duration of second period for 'high'
            /// \param reset_level Reset level
            /// \param reset_duration Duration of reset
            RGBLedTiming(bool low_duration_0_level,
                         uint16_t low_duration_0,
                         uint16_t low_duration_1,
                         bool high_duration_0_level,
                         uint16_t high_duration_0,
                         uint16_t high_duration_1,
                         bool reset_level,
                         uint32_t reset_duration)
                    :
                      low_duration_0_level(low_duration_0_level),
                      low_duration_0(low_duration_0),
                      low_duration_1(low_duration_1),
                      high_duration_0_level(high_duration_0_level),
                      high_duration_0(high_duration_0),
                      high_duration_1(high_duration_1),
                      reset_level(reset_level),
                      reset_duration(reset_duration)
            {
            }

        public:
            rmt_item32_t get_low() const
            {
                rmt_item32_t t;
                t.duration0 = low_duration_0;
                t.level0 = low_duration_0_level;
                t.duration1 = low_duration_1;
                t.level1 = !low_duration_0_level;

                return t;
            }

            rmt_item32_t get_high() const
            {
                rmt_item32_t t;
                t.duration0 = high_duration_0;
                t.level0 = high_duration_0_level;
                t.duration1 = high_duration_1;
                t.level1 = !high_duration_0_level;

                return t;
            }

            rmt_item32_t get_reset() const
            {
                rmt_item32_t t;
                t.duration0 = reset_duration;
                t.level0 = reset_level;
                t.duration1 = 0;
                t.level1 = reset_level;

                return t;
            }

        private:
            bool low_duration_0_level;
            uint16_t low_duration_0;
            uint16_t low_duration_1;
            bool high_duration_0_level;
            uint16_t high_duration_0;
            uint16_t high_duration_1;
            bool reset_level;
            uint32_t reset_duration;
    };

    /// Timings for WS2812B
    class WS2812B
        : public RGBLedTiming
    {
        public:
            WS2812B()
                    : RGBLedTiming(
                          true, 400, 850,
                          true, 800, 450,

                          // The specification states >= 50us, but empirical testing has shown
                          // that using exactly 50us isn't reliable.
                          false, 55000)
            {
            }
    };

    /// Timings for WS2812
    /// @note Untested
    class WS2812
        : public RGBLedTiming
    {
        public:
            WS2812()
                    : RGBLedTiming(
                          true, 350, 800,
                          true, 700, 600,
                          false, 50000)
            {
            }
    };

    /// Timings for SK6812 (untested)
    /// @note Untested
    class SK6812
        : public RGBLedTiming
    {
        public:
            SK6812()
                    : RGBLedTiming(
                          true, 300, 900,
                          true, 600, 600,
                          false, 80000)
            {
            }
    };

    /// Timings for WS2813 (untested)
    /// @note Untested
    class WS2813
        : public RGBLedTiming
    {
        public:
            WS2813()
                    : RGBLedTiming(
                          true, 350, 350,
                          true, 800, 350,
                          false, 300000)
            {
            }
    };
}

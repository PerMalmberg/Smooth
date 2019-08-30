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

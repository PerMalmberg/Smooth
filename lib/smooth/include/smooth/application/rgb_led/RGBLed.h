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
#include <vector>
#include "RGBLedTiming.h"

namespace smooth::application::rgb_led
{
    class RGBLed
    {
        public:
            RGBLed(rmt_channel_t channel, gpio_num_t io_pin, uint16_t led_count, const RGBLedTiming& timing);

            void set_pixel(uint16_t ix, uint8_t red, uint8_t green, uint8_t blue);

            void clear();

            void apply();

            virtual ~RGBLed();

        private:
            static constexpr double pulse_width = 12.5; // ns, i.e. 1 / 80MHz

            static constexpr uint16_t t0h = 400; // ns
            static constexpr uint16_t t0l = 850; // ns
            static constexpr uint16_t t1h = 800; // ns
            static constexpr uint16_t t1l = 450; // ns
            static constexpr uint16_t reset_time = 55000; // ns = 50us

            static constexpr uint8_t clock_divider = 4;
            static constexpr uint8_t bits_per_pixel = 24;

            uint16_t get_rmt_duration(uint16_t duration_in_ns);

            void add_color(std::vector<rmt_item32_t>::iterator& pixel, uint8_t color);

            std::vector<rmt_item32_t> led_data;
            rmt_channel_t channel;
            uint16_t led_count;
            rmt_item32_t hi;
            rmt_item32_t low;
            rmt_item32_t reset;
    };
}

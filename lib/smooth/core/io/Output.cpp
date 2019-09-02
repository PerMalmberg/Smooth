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

#include <smooth/core/io/Output.h>

namespace smooth::core::io
{
    Output::Output(gpio_num_t io, bool active_high, bool pull_up, bool pull_down, bool clear_on_creation)
            : io(io),
              active_high(active_high)
    {
        gpio_config_t config;

        config.pin_bit_mask = 1;
        config.pin_bit_mask <<= io;

        config.mode = active_high ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT_OUTPUT_OD;
        config.pull_down_en = pull_down ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
        config.pull_up_en = pull_up ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
        config.intr_type = GPIO_INTR_DISABLE;

        ESP_ERROR_CHECK(gpio_config(&config));

        if (clear_on_creation)
        {
            clr();
        }
    }

    void Output::set()
    {
        gpio_set_level(io, active_high ? 1 : 0);
    }

    void Output::set(bool active)
    {
        if (active)
        {
            set();
        }
        else
        {
            clr();
        }
    }

    void Output::clr()
    {
        gpio_set_level(io, active_high ? 0 : 1);
    }
}

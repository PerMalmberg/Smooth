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

#include <smooth/core/io/Input.h>
#include <cstring>

namespace smooth::core::io
{
    Input::Input(gpio_num_t io)
            : Input(io, false, true)
    {
    }

    Input::Input(gpio_num_t io, bool pull_up, bool pull_down, gpio_int_type_t interrupt_type)
            : io(io)
    {
        gpio_config_t config;

        config.pin_bit_mask = 1;
        config.pin_bit_mask <<= io;

        config.mode = GPIO_MODE_INPUT;
        config.pull_down_en = pull_down ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
        config.pull_up_en = pull_up ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
        config.intr_type = interrupt_type;

        ESP_ERROR_CHECK(gpio_config(&config));
    }

    bool Input::read()
    {
        return gpio_get_level(io) == 1;
    }
}

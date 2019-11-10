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

#include "smooth/core/io/Input.h"
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

    bool Input::operator==(const Input& other)
    {
        return io == other.io;
    }

    bool Input::operator!=(const Input& other)
    {
        return !(*this == other);
    }
}

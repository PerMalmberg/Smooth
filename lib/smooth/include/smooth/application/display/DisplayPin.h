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

#include "smooth/core/io/Output.h"

namespace smooth::application::display
{
    class DisplayPin
    {
        public:
            /// Constructor
            DisplayPin(gpio_num_t pin,
                       bool pullup,
                       bool pulldn,
                       bool active_high) : display_pin(pin, true, pullup, pulldn)
            {
                active_high ? display_pin.set(false) : display_pin.set(true);
            }

            /// Set output pin level
            void set_output_level(bool level)
            {
                display_pin.set(level);
            }

        private:
            smooth::core::io::Output display_pin;
    };
}

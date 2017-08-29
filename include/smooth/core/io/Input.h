//
// Created by permal on 8/18/17.
//

#pragma once

#include <driver/gpio.h>

namespace smooth
{
    namespace core
    {
        namespace io
        {
            class Input
            {
                public:
                    Input(gpio_num_t io);

                    Input(gpio_num_t io, gpio_pull_mode_t pull);

                    bool read();

                    Input(const Input&) = delete;
                    Input& operator=(const Input&) = delete;

                private:
                    gpio_num_t io;
            };
        }
    }
}

//
// Created by permal on 8/18/17.
//

#pragma once

#include <driver/gpio.h>
#include <functional>

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

                    Input(gpio_num_t io,
                          bool pull_up,
                          bool pull_down,
                          gpio_int_type_t interrupt_type = GPIO_INTR_DISABLE);

                    bool read();

                    Input(const Input&) = delete;
                    Input& operator=(const Input&) = delete;

                protected:
                    gpio_num_t io;
            };
        }
    }
}

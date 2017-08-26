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
            class Output
            {
                public:
                    Output(gpio_num_t io, bool active_high, bool clear_on_creation = true) : io(io),
                                                                                             active_high(active_high)
                    {
                        gpio_set_direction(io, active_high ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT_OUTPUT_OD);
                        if (clear_on_creation)
                        {
                            clr();
                        }
                    }

                    void set()
                    {
                        gpio_set_level(io, 1);
                    }

                    void clr()
                    {
                        gpio_set_level(io, 0);
                    }

                    Output(const Output&) = delete;
                    Output& operator=(const Output&) = delete;

                private:
                    gpio_num_t io;
                    bool active_high;
            };
        }
    }
}

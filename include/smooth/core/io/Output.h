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
                    Output(gpio_num_t io, bool active_high, bool clear_on_creation = true);

                    void set();

                    void clr();

                    Output(const Output&) = delete;
                    Output& operator=(const Output&) = delete;

                private:
                    gpio_num_t io;
                    bool active_high;
            };
        }
    }
}

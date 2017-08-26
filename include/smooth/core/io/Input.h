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
                    Input(gpio_num_t io) : io(io)
                    {
                        gpio_set_direction(io, GPIO_MODE_INPUT);
                    }

                    Input(gpio_num_t io, gpio_pull_mode_t pull)
                            : Input(io)
                    {
                        gpio_set_pull_mode(io, pull);
                    }

                    bool read()
                    {
                        /*
                        bool res;
                        if (io < GPIO_NUM_32)
                        {
                            uint32_t value = gpio_input_get();
                            res = (value & io) > 0;
                            ESP_LOGV("Read", "%x", value);
                        }
                        else
                        {
                            // Bit 0 for GPIO32
                            uint32_t value = gpio_input_get_high();
                            uint32_t bit = 1 << (io - GPIO_NUM_32);
                            res = (value & bit) > 0;
                            ESP_LOGV("Read", "%x", value);
                        }

                        return res;
                         */
                        return gpio_get_level(io) == 1;
                    }

                    Input(const Input&) = delete;
                    Input& operator=(const Input&) = delete;

                private:
                    gpio_num_t io;
            };
        }
    }
}

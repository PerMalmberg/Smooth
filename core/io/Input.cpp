//
// Created by permal on 8/29/17.
//

#include <smooth/core/io/Input.h>

namespace smooth
{
    namespace core
    {
        namespace io
        {
            Input::Input(gpio_num_t io)
                    : Input(io, false, true)
            {
            }

            Input::Input(gpio_num_t io, bool pull_up, bool pull_down)
                    : Input(io)
            {
                gpio_config_t config;
                config.pin_bit_mask = 1 << io;
                config.mode = GPIO_MODE_INPUT;
                config.pull_down_en = pull_down ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
                config.pull_up_en = pull_up ? GPIO_PULLUP_ENABLE :GPIO_PULLUP_DISABLE;
                config.intr_type = GPIO_INTR_DISABLE;

                gpio_config(&config);
            }

            bool Input::read()
            {
                return gpio_get_level(io) == 1;
            }
        }
    }
}
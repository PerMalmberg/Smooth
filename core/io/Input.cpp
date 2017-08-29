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
            Input::Input(gpio_num_t io) : io(io)
            {
                gpio_set_direction(io, GPIO_MODE_INPUT);
            }

            Input::Input(gpio_num_t io, gpio_pull_mode_t pull)
                    : Input(io)
            {
                gpio_set_pull_mode(io, pull);
            }

            bool Input::read()
            {
                return gpio_get_level(io) == 1;
            }
        }
    }
}
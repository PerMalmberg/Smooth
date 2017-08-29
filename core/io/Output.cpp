//
// Created by permal on 8/29/17.
//

#include <smooth/core/io/Output.h>

namespace smooth
{
    namespace core
    {
        namespace io
        {
            Output::Output(gpio_num_t io, bool active_high, bool clear_on_creation)
                    : io(io),
                      active_high(active_high)
            {
                gpio_set_direction(io, active_high ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT_OUTPUT_OD);
                if (clear_on_creation)
                {
                    clr();
                }
            }

            void Output::set()
            {
                gpio_set_level(io, 1);
            }

            void Output::clr()
            {
                gpio_set_level(io, 0);
            }
        };
    }
}

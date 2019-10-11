#pragma once

#include <driver/gpio.h>

namespace smooth::core::io
{
    class InterruptInputEvent
    {
        public:
            InterruptInputEvent() = default;

            InterruptInputEvent(gpio_num_t io, bool state)
                    : io(io), state(state)
            {
            }

            [[nodiscard]] bool get_state() const
            {
                return state;
            }

            [[nodiscard]] gpio_num_t get_io() const
            {
                return io;
            }

        private:
            gpio_num_t io;
            bool state;
    };
}

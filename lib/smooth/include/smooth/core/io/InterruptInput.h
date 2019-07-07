// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "Input.h"
#include <smooth/core/ipc/IISRTaskEventQueue.h>

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

            bool get_state() const
            {
                return state;
            }

            gpio_num_t get_io() const
            {
                return io;
            }

        private:
            gpio_num_t io;
            bool state;
    };

    class InterruptInput
            : private Input
    {
        public:

            /// Constructs an InterruptInput
            /// \param queue The queue to put events on
            /// \param io GPIO pin number
            /// \param pull_up Set to true if the input has a pull-up (also enables the internal pull up)
            /// \param pull_down Set to true if the input has a pull-down (also enables the internal pull-down)
            /// \param interrupt_trigger When the interrupt should trigger
            InterruptInput(core::ipc::IISRTaskEventQueue<InterruptInputEvent>& queue, gpio_num_t io,
                           bool pull_up,
                           bool pull_down,
                           gpio_int_type_t interrupt_trigger
            );

            void update();

            gpio_num_t get_io() const
            {
                return io;
            }

        private:
            core::ipc::IISRTaskEventQueue<InterruptInputEvent>& queue;
    };
}

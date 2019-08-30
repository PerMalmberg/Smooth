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

#include <smooth/core/io/InterruptInput.h>
#include <driver/gpio.h>
#include <chrono>

namespace smooth::core::io
{
    static void input_interrupt_handler(void* arg)
    {
        auto ev_input = static_cast<InterruptInput*>(arg);

        if (ev_input != nullptr)
        {
            ev_input->update();
        }
    }

    InterruptInput::InterruptInput(std::weak_ptr<core::ipc::IISRTaskEventQueue<InterruptInputEvent>> queue,
                                   gpio_num_t io,
                                   bool pull_up,
                                   bool pull_down,
                                   gpio_int_type_t interrupt_trigger)
            : Input(io, pull_up, pull_down, interrupt_trigger), queue(std::move(queue))
    {
        gpio_isr_handler_add(io, input_interrupt_handler, this);
    }

    void InterruptInput::update()
    {
        InterruptInputEvent ev(io, read());
        const auto& q = queue.lock();

        if (q)
        {
            q->signal(ev);
        }
    }
}

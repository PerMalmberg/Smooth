/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "smooth/core/io/InterruptInput.h"
#include <driver/gpio.h>

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

    InterruptInput::~InterruptInput()
    {
        gpio_isr_handler_remove(get_io());
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

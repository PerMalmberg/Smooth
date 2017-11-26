//
// Created by permal on 9/1/17.
//

#include <smooth/core/io/InterruptInput.h>
#include <driver/gpio.h>
#include <chrono>

namespace smooth
{
    namespace core
    {
        namespace io
        {
            static void input_interrupt_handler(void* arg)
            {
                InterruptInput* ev_input = static_cast<InterruptInput*>(arg);
                if (ev_input != nullptr)
                {
                    ev_input->signal();
                }
            }

            InterruptInput::InterruptInput(core::ipc::IISRTaskEventQueue<InterruptInputEvent>& queue, gpio_num_t io,
                                           bool pull_up, bool pull_down, gpio_int_type_t interrupt_trigger)
                    : Input(io, pull_up, pull_down, interrupt_trigger), queue(queue)
            {
                gpio_isr_handler_add(io, input_interrupt_handler, this);
            }

            void InterruptInput::signal()
            {
                InterruptInputEvent ev(io, read());
                queue.signal(ev);
            }
        }
    }
}
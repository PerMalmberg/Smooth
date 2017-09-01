//
// Created by permal on 9/1/17.
//

#include <smooth/core/io/EventInput.h>
#include <smooth/core/ipc/Publisher.h>
#include <driver/gpio.h>

namespace smooth
{
    namespace core
    {
        namespace io
        {
            void input_interrupt_handler(void* arg)
            {
                EventInput* ev_input = static_cast<EventInput*>(arg);
                if (ev_input != nullptr)
                {
                    ev_input->signal();
                }
            }

            EventInput::EventInput(core::ipc::TaskEventQueue<InputInterruptEventEvent>& queue, gpio_num_t io,
                                   bool pull_up, bool pull_down)
                    : Input(io, pull_up, pull_down, pull_up ? GPIO_INTR_NEGEDGE : GPIO_INTR_POSEDGE), queue(queue)
            {
                gpio_isr_handler_add(io, input_interrupt_handler, this);
            }

            void EventInput::signal()
            {
                InputInterruptEventEvent item(io, read());
                queue.push_from_isr(item);
            }
        }
    }
}
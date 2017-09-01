//
// Created by permal on 9/1/17.
//

#pragma once

#include "Input.h"
#include <smooth/core/ipc/TaskEventQueue.h>

namespace smooth
{
    namespace core
    {
        namespace io
        {
            class InputInterruptEventEvent
            {
                public:
                    InputInterruptEventEvent() = default;

                    InputInterruptEventEvent(gpio_num_t io, bool state)
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

            class EventInput
                    : public Input
            {
                public:
                    EventInput(core::ipc::TaskEventQueue<InputInterruptEventEvent>& queue, gpio_num_t io,
                               bool pull_up,
                               bool pull_down
                    );

                    void signal();
                private:
                    core::ipc::TaskEventQueue<InputInterruptEventEvent>& queue;
            };
        }
    }
}

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
            ///
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
                    InterruptInput(core::ipc::TaskEventQueue<InterruptInputEvent>& queue, gpio_num_t io,
                                   bool pull_up,
                                   bool pull_down
                    );

                    void signal();

                    gpio_num_t get_io() const
                    {
                        return io;
                    }

                private:
                    core::ipc::TaskEventQueue<InterruptInputEvent>& queue;
            };
        }
    }
}

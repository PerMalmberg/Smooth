//
// Created by permal on 2018-10-21.
//

#include "interrupt_queue.h"
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>

using namespace smooth::core;

namespace interrupt_queue
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1)),
              queue(*this, *this),
              input(queue, GPIO_NUM_21, true, false, GPIO_INTR_ANYEDGE)
    {
    }

    void App::init()
    {
        std::cout << "Trigger the input!" << std::endl;
    }

    void App::event(const smooth::core::io::InterruptInputEvent& value)
    {
        std::cout << "Value from interrupt: " << value.get_state() << std::endl;
    }
}
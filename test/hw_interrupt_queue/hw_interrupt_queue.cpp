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

#include "hw_interrupt_queue.h"
#include "smooth/core/Task.h"
#include "smooth/core/task_priorities.h"

using namespace smooth::core;

namespace hw_interrupt_queue
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1)),
              queue(IntrQueue::create(*this, *this)),
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

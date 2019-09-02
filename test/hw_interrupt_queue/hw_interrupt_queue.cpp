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

#include "hw_interrupt_queue.h"
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>

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

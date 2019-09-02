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

#include "hello_world.h"
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>

using namespace smooth::core;

namespace hello_world
{
    class ATask : public smooth::core::Task
    {
        public:
            ATask() : smooth::core::Task("Other task", 9000, APPLICATION_BASE_PRIO, std::chrono::seconds{ 1 })
            {}

            void tick() override
            {
                std::cout << "Hello from other task" << std::endl;
            }
    }

    a_instance;

    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1))
    {
    }

    void App::init()
    {
        std::cout << "Hello world!" << std::endl;
        a_instance.start();
    }

    void App::tick()
    {
        std::cout << "Tick!" << std::endl;
    }
}

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

#include "starter_example.h"
#include "smooth/core/Task.h"
#include "smooth/core/task_priorities.h"
#include "smooth/core/SystemStatistics.h"

using namespace smooth::core;

namespace starter_example
{
    class ATask : public smooth::core::Task
    {
        public:
            ATask() : smooth::core::Task("Other task", 9000, APPLICATION_BASE_PRIO, std::chrono::seconds{ 1 })
            {}

            void tick() override
            {
                Log::info("App::Init", "Hello from other task");
            }
    };

    ATask a_instance{};

    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(3))
    {
    }

    void App::init()
    {
        Log::info("App::Init", "Starting...");
        a_instance.start();
    }

    void App::tick()
    {
        Log::info("App", "Hello world!");
        SystemStatistics::instance().dump();
    }
}

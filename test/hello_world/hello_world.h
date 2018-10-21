#pragma once

#include <smooth/core/Application.h>
#include <smooth/core/task_priorities.h>
#include <iostream>

namespace hello_world
{
    class App : public smooth::core::Application
    {
    public:
        App() : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1))
        {
        }

        void tick() override
        {
            std::cout << "Hello world!" << std::endl;
        }
    };
}
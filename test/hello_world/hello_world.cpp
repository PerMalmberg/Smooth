//
// Created by permal on 2018-10-21.
//

#include "hello_world.h"
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>

using namespace smooth::core;

namespace hello_world
{

    class ATask : public smooth::core::Task
    {
        public:
            ATask() : smooth::core::Task("Other task", 9000, APPLICATION_BASE_PRIO, std::chrono::seconds{1})
            {}

            void tick() override
            {
                std::cout << "Hello from other task" << std::endl;
            }
    } a_instance;


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
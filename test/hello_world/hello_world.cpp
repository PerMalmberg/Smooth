//
// Created by permal on 2018-10-21.
//

#include "hello_world.h"

namespace hello_world
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1))
    {
    }

    void App::init()
    {
        std::cout << "Hello world!" << std::endl;
    }

    void App::tick()
    {
        std::cout << "Tick!" << std::endl;
    }
}
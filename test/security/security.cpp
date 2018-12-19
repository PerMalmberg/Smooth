//
// Created by permal on 2018-10-21.
//

#include "security.h"
#include <iostream>
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/timer/ElapsedTime.h>

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::timer;
using namespace smooth::application::security;

namespace security
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1))
    {
    }

    void App::init()
    {
        Application::init();
        constexpr char* short_string = "Password";
        constexpr char* long_string = "A freaking long string of a password. Its so loooooong I can't remember what I wote at the begining of the string";

        time(short_string, 3);
        time(short_string, 4);
        time(short_string, 5);
        time(short_string, 6);
        time(short_string, 7);
        time(short_string, 8);
        std::cout << "----------" << std::endl;
        time(long_string, 3);
        time(long_string, 4);
        time(long_string, 5);
        time(long_string, 6);
        time(long_string, 7);
        time(long_string, 8);
        std::cout << "----------" << std::endl;
    }

    void App::tick()
    {
        
    }

    void App::time(const std::string& password, size_t ops)
    {
        ElapsedTime t;
        t.start();
        auto res = ph.hash_to_storage(password, ops);
        auto elapsed = duration_cast<milliseconds>(t.get_running_time()).count();
        std::cout << ops << "\t" << elapsed << "ms\t" << std::get<0>(res) << " " << std::get<1>(res) << std::endl;
    }

}
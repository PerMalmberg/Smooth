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
        const std::string short_string{"Password"};
        const std::string long_string{"A freaking long string of a password. Its so loooooong I can't remember what I wote at the begining of the string"};

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
        auto res = ph.hash(password, ops);
        auto elapsed = duration_cast<milliseconds>(t.get_running_time()).count();
        auto verified = ph.verify_password_against_hash(password, std::get<1>(res));
        auto not_verified = ph.verify_password_against_hash("Wrong pass", std::get<1>(res));
        std::cout << ops << "\t" << elapsed << "ms\t" << std::get<0>(res) << " " << std::get<1>(res) << " verified: " << verified << " not verified: " << !not_verified << std::endl;
    }

}
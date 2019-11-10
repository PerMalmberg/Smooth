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

#include "security.h"
#include <iostream>
#include "smooth/core/Task.h"
#include "smooth/core/task_priorities.h"
#include "smooth/core/timer/ElapsedTime.h"

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
        const std::string short_string{ "Password" };
        const std::string long_string{
            "A freaking long string of a password. Its so loooooong I can't remember what I wrote at the beginning of the string" };

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
        const auto& [result, hash] = ph.hash(password, ops);
        auto elapsed = duration_cast<milliseconds>(t.get_running_time()).count();
        auto verified = ph.verify_password_against_hash(password, hash);
        auto not_verified = ph.verify_password_against_hash("Wrong pass", hash);
        std::cout << ops << "\t" << elapsed << "ms\t" << result << " " << hash << " verified: "
                  << verified << " not verified: " << !not_verified << std::endl;
    }
}

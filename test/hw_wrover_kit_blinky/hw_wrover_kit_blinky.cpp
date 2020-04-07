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

#include "hw_wrover_kit_blinky.h"

#include "smooth/core/task_priorities.h"

using namespace smooth;
using namespace smooth::core;
using namespace std::chrono;

namespace hw_wrover_kit_blinky
{
    App::App()
            : Application(APPLICATION_BASE_PRIO,
                          milliseconds(300))
    {
    }

    void App::init()
    {
        Application::init();
        Log::info("LED fun", "Lets blink some LEDs");
    }

    void App::tick()
    {
        // Toggle LEDs to represent the current state value.
        g.set(static_cast<bool>(state & 1));
        r.set(static_cast<bool>(state & 2));
        b.set(static_cast<bool>(state & 4));

        state++;
    }
}

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

#ifndef ESP_PLATFORM
#error "This example requires H/W to run on and can only be compiled with IDF."
#endif

#include "hw_wrover_kit_blinky.h"

#include <smooth/core/task_priorities.h>

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
        Log::info("LED fun", Format("Lets blink some LEDs"));
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

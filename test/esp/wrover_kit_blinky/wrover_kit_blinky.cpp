//
// Created by permal on 2018-10-26.
//

#ifndef ESP_PLATFORM
#error "This example requires H/W to run on and can only be compiled with IDF."
#endif

#include "wrover_kit_blinky.h"

#include <smooth/core/task_priorities.h>

using namespace smooth;
using namespace smooth::core;
using namespace std::chrono;

namespace wrover_kit_blinky
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
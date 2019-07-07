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

#pragma once

#include <smooth/core/Application.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/timer/Timer.h>
#include <smooth/core/io/Output.h>

namespace wrover_kit_blinky
{
    // This app is tested on Wrover Kit v3.
    class App
            : public smooth::core::Application
    {
    public:

        App();

        void init() override;

        void tick() override;

    private:
        smooth::core::io::Output r{GPIO_NUM_0, true, false, false, false};
        smooth::core::io::Output g{GPIO_NUM_2, true, false, false, false};
        smooth::core::io::Output b{GPIO_NUM_4, true, false, false, false};
        uint8_t state = 0;
    };
}
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

#pragma once

#include "smooth/core/Application.h"
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/timer/Timer.h"
#include "smooth/core/io/Output.h"

namespace hw_wrover_kit_blinky
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
            smooth::core::io::Output r{ GPIO_NUM_0, true, false, false, false };
            smooth::core::io::Output g{ GPIO_NUM_2, true, false, false, false };
            smooth::core::io::Output b{ GPIO_NUM_4, true, false, false, false };
            uint8_t state = 0;
    };
}

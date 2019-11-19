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

#include <vector>
#include <chrono>
#include <unordered_map>
#include "smooth/core/ipc/Queue.h"
#include "smooth/core/Task.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/ipc/SubscribingTaskEventQueue.h"
#include "smooth/core/network/Wifi.h"

namespace smooth::core
{
    /// EarlyInit is used to initialize things that needs to be initialized before the Application.
    class EarlyInit : public Task
    {
        public:
            EarlyInit(uint32_t priority, const std::chrono::milliseconds& tick_interval);

            ~EarlyInit() override;
    };

    /// The Application 'attaches' itself to the main task and gives the application programmer
    /// the same possibilities to perform work on the main task as if a separate Task had been created.
    /// Any application written based on Smooth should have an instance of the Application
    /// class (or a class derived from Application) on the stack in its app_main().
    /// Be sure to adjust the stack size of the main task accordingly using 'make menuconfig'.
    /// Note: Unlike the version of start() in Task, when called on an Application instance start() never returns.
    class Application
        : public EarlyInit
    {
        public:
            /// Constructor
            /// \param priority The priority to run at. Usually smooth::core::APPLICATION_BASE_PRIO
            /// + an arbitrary value, but should be lower than the priority of the ESP-IDFs task
            /// such as the Wifi driver.
            /// \param tick_interval The tick interval
            Application(uint32_t priority, std::chrono::milliseconds tick_interval)
                    : EarlyInit(priority, tick_interval)
            {
            }

            void init() override;

            network::Wifi& get_wifi()
            {
                return wifi;
            }

        private:
            network::Wifi wifi{};
    };
}

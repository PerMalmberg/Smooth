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
namespace timer
{
    class App
        : public smooth::core::Application,
        public smooth::core::ipc::IEventListener<smooth::core::timer::TimerExpiredEvent>
    {
        public:
            App();

            void init() override;

            uint32_t count = 0;

            void event(const smooth::core::timer::TimerExpiredEvent& event) override;

        private:
            void create_timer(std::chrono::milliseconds interval);

            struct TimerInfo
            {
                smooth::core::timer::TimerOwner timer;
                std::chrono::milliseconds interval;
                std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
                int count = 0;
                std::chrono::milliseconds total = std::chrono::milliseconds(0);
            };

            using ExpiredQueue = smooth::core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent>;
            std::shared_ptr<ExpiredQueue> queue;
            std::vector<TimerInfo> timers;
    };
}

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
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/timer/ElapsedTime.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/Application.h"

namespace task_event_queue
{
    using ElapsedTimeQueue = smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime>;

    class SenderTask
        : public smooth::core::Task
    {
        public:
            explicit SenderTask(std::weak_ptr<ElapsedTimeQueue> out);

            void tick() override;

        private:
            std::weak_ptr<smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime>> out;
    };

    class App
        : public smooth::core::Application,
        public smooth::core::ipc::IEventListener<smooth::core::timer::ElapsedTime>
    {
        public:
            App();

            void init() override;

            uint32_t count = 0;
            std::chrono::microseconds total = std::chrono::microseconds(0);

            void event(const smooth::core::timer::ElapsedTime& event) override;

        private:
            std::shared_ptr<ElapsedTimeQueue> queue;
            SenderTask sender;
    };
}

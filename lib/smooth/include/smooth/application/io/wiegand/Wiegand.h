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

#include <bitset>
#include <smooth/core/Task.h>
#include <smooth/core/io/InterruptInput.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/ISRTaskEventQueue.h>
#include <smooth/core/ipc/ITaskEventQueue.h>
#include <smooth/core/timer/Timer.h>
#include <memory>
#include "IWiegandSignal.h"

namespace smooth::application::io::wiegand
{
    class Wiegand
        : public smooth::core::ipc::IEventListener<smooth::core::io::InterruptInputEvent>,
        public smooth::core::ipc::IEventListener<smooth::core::timer::TimerExpiredEvent>
    {
        public:
            Wiegand(smooth::core::Task& task, IWiegandSignal& receiver, gpio_num_t d0_pin, gpio_num_t d1_pin);

            void event(const smooth::core::io::InterruptInputEvent& event) override;

            void event(const smooth::core::timer::TimerExpiredEvent& event) override;

        private:
            IWiegandSignal& receiver;
            gpio_num_t d0_pin;
            gpio_num_t d1_pin;
            using ISRTaskQueue = smooth::core::ipc::ISRTaskEventQueue<smooth::core::io::InterruptInputEvent, 40>;
            std::shared_ptr<ISRTaskQueue> bit_queue;
            smooth::core::io::InterruptInput d0;
            smooth::core::io::InterruptInput d1;
            std::bitset<34> data{};
            uint8_t bit_count = 0;

            using TimerQueue = smooth::core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent>;
            std::shared_ptr<TimerQueue> line_silent;
            smooth::core::timer::TimerOwner expire;
    };
}

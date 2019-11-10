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
#include <memory>
#include "smooth/core/Task.h"
#include "smooth/core/io/InterruptInputCB.h"
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/ipc/ISRTaskEventQueue.h"
#include "smooth/core/ipc/ITaskEventQueue.h"
#include "smooth/core/timer/Timer.h"
#include <esp_timer.h>
#include "IWiegandSignal.h"

namespace smooth::application::io::wiegand
{
    class Wiegand : public smooth::core::ipc::IEventListener<uint8_t>,
        public smooth::core::ipc::IEventListener<uint32_t>,
        public smooth::core::ipc::IEventListener<smooth::core::timer::TimerExpiredEvent>
    {
        public:
            Wiegand(smooth::core::Task& task, IWiegandSignal& receiver, gpio_num_t d0_pin, gpio_num_t d1_pin);

            void zero();

            void one();

            void event(const uint8_t&) override;

            void event(const uint32_t&) override;

            void event(const smooth::core::timer::TimerExpiredEvent& event) override;

        private:
            void clear_bits();

            void parse();

            static void isr_d0(void* context);

            static void isr_d1(void* context);

            IWiegandSignal& receiver;
            gpio_num_t d0_pin;
            gpio_num_t d1_pin;
            using KeyQueue = smooth::core::ipc::ISRTaskEventQueue<uint8_t, 5>;
            std::shared_ptr<KeyQueue> key_queue;
            using NumberQueue = smooth::core::ipc::ISRTaskEventQueue<uint32_t, 5>;
            std::shared_ptr<NumberQueue> number_queue;
            using TimerQueue = smooth::core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent>;
            std::shared_ptr<TimerQueue> tick_queue;
            smooth::core::timer::TimerOwner tick;
            smooth::core::io::InterruptInputCB d0;
            smooth::core::io::InterruptInputCB d1;
            std::bitset<34> data{};
            uint8_t bit_count = 0;

            using clock = std::chrono::high_resolution_clock;
            int64_t last_data = esp_timer_get_time();
    };
}

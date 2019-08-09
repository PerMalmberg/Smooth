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
            std::weak_ptr<TimerQueue> line_silent;
            std::shared_ptr<smooth::core::timer::Timer> expire;
    };
}
        
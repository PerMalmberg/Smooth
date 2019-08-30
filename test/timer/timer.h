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

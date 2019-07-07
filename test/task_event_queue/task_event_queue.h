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
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/timer/ElapsedTime.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/Application.h>

namespace task_event_queue
{
    class SenderTask
            : public smooth::core::Task
    {
    public:
        explicit SenderTask(smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime>& out);

        void tick() override;

    private:
        smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime>& out;
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
        smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime> queue;
        SenderTask sender;
    };
}
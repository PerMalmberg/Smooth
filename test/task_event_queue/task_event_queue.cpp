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

#include "task_event_queue.h"

#include <smooth/core/logging/log.h>
#include <smooth/core/task_priorities.h>

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace task_event_queue
{

    App::App()
            : Application(APPLICATION_BASE_PRIO,
                          std::chrono::milliseconds(1000)),
              queue("string queue", 10, *this, *this),
              sender(queue)
    {
    }

    void App::init()
    {
        Application::init();
        sender.start();
    }

    void App::event(const timer::ElapsedTime& event)
    {
        auto val = event.get_running_time();
        total += val;
        count++;

        if (count % 100 == 0)
        {
            Log::debug("Stats", Format("Avg: {1}us per message, Count: {2}, Last msg: {3}us",
                                       Double(static_cast<double>(total.count()) / count, 2),
                                       Int64(count),
                                       Int64(val.count())));
        }
    }

    SenderTask::SenderTask(TaskEventQueue <timer::ElapsedTime>& out) :
            core::Task("SenderTask", 4096, 10, milliseconds(1)),
            out(out)
    {
    }

    void SenderTask::tick()
    {
        timer::ElapsedTime e;
        e.start();
        out.push(e);
    }
}
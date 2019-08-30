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

#include "timer.h"

#include <smooth/core/task_priorities.h>

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::timer;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace timer
{
    App::App()
            : Application(APPLICATION_BASE_PRIO,
                          seconds(10)),
              queue(ExpiredQueue::create("queue", 10, *this, *this)),
              timers()
    {
    }

    void App::init()
    {
        Application::init();

        create_timer(milliseconds(100));
        create_timer(milliseconds(500));
        create_timer(milliseconds(1000));
        create_timer(milliseconds(3000));
        create_timer(milliseconds(5000));
        create_timer(milliseconds(10000));

        for (auto& t : timers)
        {
            t.timer->start();
        }

        timers.front().timer->stop();
        timers.front().timer->start();
    }

    void App::event(const smooth::core::timer::TimerExpiredEvent& event)
    {
        auto& info = timers[static_cast<decltype(timers)::size_type>(event.get_id())];
        milliseconds duration = duration_cast<milliseconds>(steady_clock::now() - info.last);
        info.last = steady_clock::now();
        info.count++;
        info.total += duration;

        Log::verbose("Interval", Format("{1} ({2}ms): {3}ms, avg: {4}",
                                        Int32(event.get_id()),
                                        Int64(info.interval.count()),
                                        Int64(duration.count()),
                                        Double(static_cast<double>(info.total.count()) / info.count)));
    }

    void App::create_timer(std::chrono::milliseconds interval)
    {
        TimerInfo t;
        t.timer = Timer::create("Timer", static_cast<int32_t>(timers.size()), queue, true, interval);
        t.interval = interval;
        timers.push_back(t);
    }
}

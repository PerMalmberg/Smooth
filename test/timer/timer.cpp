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

#include "timer.h"

#include "smooth/core/task_priorities.h"

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
              queue(ExpiredQueue::create(10, *this, *this)),
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

        Log::verbose("Interval", "{} ({}ms): {}ms, avg: {}",
                     event.get_id(),
                     info.interval.count(),
                     duration.count(),
                     static_cast<double>(info.total.count()) / info.count);
    }

    void App::create_timer(std::chrono::milliseconds interval)
    {
        TimerInfo t;
        t.timer = Timer::create(static_cast<int32_t>(timers.size()), queue, true, interval);
        t.interval = interval;
        timers.push_back(t);
    }
}

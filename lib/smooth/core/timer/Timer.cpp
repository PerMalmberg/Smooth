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

#include "smooth/core/timer/Timer.h"
#include "smooth/core/timer/TimerService.h"
#include "smooth/core/util/create_protected.h"

using namespace smooth::core::logging;
using namespace smooth::core::util;
using namespace std::chrono;

namespace smooth::core::timer
{
    Timer::Timer(int id, std::weak_ptr<ipc::TaskEventQueue<TimerExpiredEvent>> event_queue,
                 bool repeating, milliseconds interval)
            : id(id),
              repeating(repeating),
              timer_interval(interval),
              queue(std::move(event_queue)),
              expire_time(steady_clock::now())
    {
        // Start the timer service when a timer is fist used.
        TimerService::start_service();
    }

    void Timer::start()
    {
        stop();
        TimerService::get().add_timer(shared_from_this());
    }

    void Timer::start(milliseconds interval)
    {
        this->timer_interval = interval;
        start();
    }

    void Timer::stop()
    {
        TimerService::get().remove_timer(shared_from_this());
    }

    void Timer::reset()
    {
        stop();
        start();
    }

    int Timer::get_id() const
    {
        return id;
    }

    void Timer::expired()
    {
        TimerExpiredEvent ev(id);
        const auto& q = queue.lock();

        if (q)
        {
            q->push(ev);
        }
    }

    TimerOwner Timer::create(int id,
                             const std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>>& event_queue,
                             bool auto_reload,
                             std::chrono::milliseconds interval)
    {
        return TimerOwner(create_protected_shared<Timer>(id, event_queue, auto_reload, interval));
    }

    std::chrono::steady_clock::time_point Timer::expires_at() const
    {
        return expire_time;
    }

    void Timer::calculate_next_execution()
    {
        expire_time = steady_clock::now() + timer_interval;
    }

    TimerOwner::TimerOwner(std::shared_ptr<Timer> t) noexcept
            : t(std::move(t))
    {}

    TimerOwner::TimerOwner(int id,
                           const std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>>& event_queue,
                           bool auto_reload,
                           std::chrono::milliseconds interval)
    {
        *this = Timer::create(id, event_queue, auto_reload, interval);
    }

    TimerOwner::~TimerOwner()
    {
        if (t)
        {
            t->stop();
        }
    }

    std::shared_ptr<Timer> TimerOwner::operator->() const noexcept
    {
        return t;
    }
}

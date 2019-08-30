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

#include <smooth/core/timer/Timer.h>
#include <smooth/core/timer/TimerService.h>
#include <smooth/core/util/create_protected.h>

using namespace smooth::core::logging;
using namespace smooth::core::util;
using namespace std::chrono;

namespace smooth::core::timer
{
    Timer::Timer(std::string name, int id, std::weak_ptr<ipc::TaskEventQueue<TimerExpiredEvent>> event_queue,
                 bool repeating, milliseconds interval)
            : timer_name(std::move(name)),
              id(id),
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

    const std::string & Timer::get_name()
    {
        return timer_name;
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

    TimerOwner Timer::create(const std::string& name,
                             int id,
                             const std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>>& event_queue,
                             bool auto_reload,
                             std::chrono::milliseconds interval)
    {
        return TimerOwner(create_protected_shared<Timer>(name, id, event_queue, auto_reload, interval));
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

    TimerOwner::TimerOwner(const std::string& name,
                           int id,
                           const std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>>& event_queue,
                           bool auto_reload,
                           std::chrono::milliseconds interval)
    {
        *this = Timer::create(name, id, event_queue, auto_reload, interval);
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

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

#include "smooth/core/timer/TimerService.h"
#include "smooth/core/timer/Timer.h"
#include "smooth/core/task_priorities.h"
#include "smooth/config_constants.h"

using namespace smooth::core::logging;
using namespace std::chrono;

namespace smooth::core::timer
{
    TimerService::TimerService()
            : Task("TimerService",
                   CONFIG_SMOOTH_TIMER_SERVICE_STACK_SIZE,
                   TIMER_SERVICE_PRIO,
                   milliseconds(0)),
              cmp([](const SharedTimer& left, const SharedTimer& right) {
                      // We want the timer with the least time left to be first in the list
                      return left->expires_at() > right->expires_at();
                  }),
              queue(cmp),
              guard()
    {
    }

    TimerService& TimerService::get()
    {
        static TimerService service;

        return service;
    }

    void TimerService::start_service()
    {
        get().start();
    }

    void TimerService::add_timer(const SharedTimer& timer)
    {
        std::lock_guard<std::mutex> lock(guard);
        timer->calculate_next_execution();
        queue.push(timer);
        cond.notify_one();
    }

    void TimerService::remove_timer(const SharedTimer& timer)
    {
        std::lock_guard<std::mutex> lock(guard);
        queue.remove_timer(timer);
        cond.notify_one();
    }

    void TimerService::tick()
    {
        std::unique_lock<std::mutex> lock(guard);

        if (queue.empty())
        {
            // No timers, wait until one is added.
            cond.wait_for(lock, seconds(1));
        }
        else
        {
            // Get a fixed 'now'
            auto now = steady_clock::now();

            std::vector<SharedTimer> processed{};

            // Process any expired timers
            while (!queue.empty() && now >= queue.top()->expires_at())
            {
                auto timer = queue.top();
                timer->expired();

                // Timer expired, remove from queue
                queue.pop();

                // Save timer to later add it, if repeating.
                // Otherwise, simply forget about it.
                if (timer->is_repeating())
                {
                    processed.push_back(timer);
                }
            }

            for (auto& t : processed)
            {
                t->calculate_next_execution();
                queue.push(t);
            }

            if (!queue.empty())
            {
                // Get next timer to expire
                auto timer = queue.top();

                // Wait for the timer to expire, or a timer to be removed or added.
                auto current_queue_length = queue.size();

                cond.wait_until(lock,
                                timer->expires_at(),
                                [current_queue_length, this]() {
                                    // Wake up if a timer has been added or removed.
                                    return current_queue_length != queue.size();
                                });
            }
        }
    }
}

//
// Created by permal on 10/22/17.
//

#include <smooth/core/timer/TimerService.h>
#include <smooth/core/timer/Timer.h>
#include <smooth/core/task_priorities.h>

using namespace smooth::core::logging;
using namespace std::chrono;

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            TimerService::TimerService()
                    : Task("TimerService",
                            1024 * 3,
                            TIMER_SERVICE_PRIO,
                            milliseconds(0)),
                            cmp([](SharedTimer left, SharedTimer right) {
                                // We want the timer with the least time left to be first in the list
                                return left->expires_at() > right->expires_at();
                            }),
                            queue(cmp),
                            guard(),
                            processed()
            {
                // Disable status printing to conserve stack size.
                disable_status_print();
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

            void TimerService::add_timer(SharedTimer timer)
            {
                std::lock_guard<std::mutex> lock(guard);
                timer->calculate_next_execution();
                queue.push(timer);
                cond.notify_one();
            }

            void TimerService::remove_timer(SharedTimer timer)
            {
                std::lock_guard<std::mutex> lock(guard);
                queue.remove_timer(timer);
                cond.notify_one();
            }

            void TimerService::tick()
            {
                std::unique_lock<std::mutex> lock(guard);
                processed.clear();

                if (queue.empty())
                {
                    // No timers, wait until one is added.
                    cond.wait_for(lock, seconds(1));
                }
                else
                {
                    // Get a fixed 'now'
                    auto now = steady_clock::now();

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
    }
}
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

#include <thread>
#include "smooth/core/ipc/QueueNotification.h"
#include <algorithm>

namespace smooth::core::ipc
{
    void QueueNotification::notify(const std::weak_ptr<ITaskEventQueue>& queue)
    {
        // It might look like the queue can grow without bounds, but that is not the case
        // as TaskEventQueues only call this method when they have successfully added the
        // data item to their internal queue. As such, the queue can only be as large as
        // the sum of all queues within the same Task.
        std::unique_lock<std::mutex> lock{ guard };
        queues.emplace_back(queue);
        cond.notify_one();
    }

    void QueueNotification::remove_expired_queues()
    {
        std::unique_lock<std::mutex> lock{ guard };

        auto new_end = std::remove_if(queues.begin(), queues.end(),
                                      [&](const auto& o) { return o.expired(); });

        queues.erase(new_end, queues.end());
    }

    std::weak_ptr<ITaskEventQueue> QueueNotification::wait_for_notification(std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock{ guard };
        std::weak_ptr<ITaskEventQueue> res{};

        if (queues.empty())
        {
            // Wait until data is available, or timeout. This will atomically release the lock.
            auto wait_result = cond.wait_until(lock,
                                               std::chrono::steady_clock::now() + timeout,
                                               [this]() {
                                                   // Stop waiting when there is data
                                                   return !queues.empty();
                                               });

            // At this point we will have the lock again.
            if (wait_result)
            {
                if (!queues.empty())
                {
                    res = queues.front();
                    queues.pop_front();
                }
            }
        }
        else
        {
            res = queues.front();
            queues.pop_front();
        }

        return res;
    }
}

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

#include <thread>
#include <smooth/core/ipc/QueueNotification.h>
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

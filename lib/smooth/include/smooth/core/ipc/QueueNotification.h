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

#pragma once

#include <condition_variable>
#include <mutex>
#include <deque>
#include <memory>
#include "ITaskEventQueue.h"

namespace smooth::core::ipc
{
    class QueueNotification
    {
        public:
            QueueNotification() = default;

            ~QueueNotification() = default;

            void notify(const std::weak_ptr<ITaskEventQueue>& queue);

            void remove_expired_queues();

            std::weak_ptr<ITaskEventQueue> wait_for_notification(std::chrono::milliseconds timeout);

            void clear()
            {
                std::lock_guard<std::mutex> lock(guard);
                queues.clear();
            }

        private:
            std::deque<std::weak_ptr<ITaskEventQueue>> queues{};
            std::mutex guard{};
            std::condition_variable cond{};
    };
}

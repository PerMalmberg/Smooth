//
// Created by permal on 10/18/17.
//

#pragma once

#include <chrono>
#include <queue>
#include <mutex>
#include <algorithm>
#include "QueueNotification.h"
#include "ITaskEventQueue.h"
#include <smooth/core/timer/ElapsedTime.h>

namespace smooth
{
    namespace core
    {
        class Task;

        namespace ipc
        {
            class QueueNotification
            {
                public:
                    QueueNotification() = default;
                    ~QueueNotification() = default;

                    void notify(ITaskEventQueue* queue);
                    ITaskEventQueue* wait_for_notification(Task* callee, std::chrono::milliseconds timeout);

                    void clear()
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        while(queues.size())
                        {
                            queues.pop();
                        }
                    }

                private:
                    std::queue<ITaskEventQueue*> queues{};
                    std::mutex guard{};
                    std::condition_variable cond{};
            };
        }
    }
}

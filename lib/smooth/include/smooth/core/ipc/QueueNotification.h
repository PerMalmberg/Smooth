//
// Created by permal on 10/18/17.
//

#pragma once

#include <condition_variable>
#include <mutex>
#include <deque>
#include "ITaskEventQueue.h"

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            class QueueNotification
            {
                public:
                    QueueNotification() = default;
                    ~QueueNotification() = default;

                    void notify(ITaskEventQueue* queue);
                    void remove(ITaskEventQueue* queue);
                    ITaskEventQueue* wait_for_notification(std::chrono::milliseconds timeout);

                    void clear()
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        queues.clear();
                    }

                private:
                    std::deque<ITaskEventQueue*> queues{};
                    std::mutex guard{};
                    std::condition_variable cond{};
            };
        }
    }
}

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
#include <smooth/core/ipc/Lock.h>

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            class QueueNotification
            {
                public:
                    QueueNotification();
                    ~QueueNotification() = default;

                    void notify(ITaskEventQueue* queue);
                    ITaskEventQueue* wait_for_notification(std::chrono::milliseconds timeout);

                    void clear()
                    {
                        core::ipc::Lock lock(guard);
                        while(queues.size())
                        {
                            queues.pop();
                        }
                    }

                private:
                    std::queue<ITaskEventQueue*> queues;
                    std::mutex guard;
            };
        }
    }
}

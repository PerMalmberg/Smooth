//
// Created by permal on 6/27/17.
//

#pragma once

#include "ITaskEventQueue.h"

namespace smooth
{
    namespace  core
    {
        namespace ipc
        {
            class QueueNotification;

            /// A queue that is polled by the owning Task, instead of itself notifying the task
            /// As an application programmer you are not meant to call any of these methods.
            class IPolledTaskQueue : public ITaskEventQueue
            {
                public:
                    /// Polls the queue, and if data is available, it will signal the owning task.
                    virtual void poll() = 0;
            };
        }
    }
}

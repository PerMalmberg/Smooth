//
// Created by permal on 6/27/17.
//

#pragma once

namespace smooth
{
    namespace  core
    {
        namespace ipc
        {
            class QueueNotification;

            /// Common interface for TaskEventQueue
            /// As an application programmer you are not meant to call any of these methods.
            class ITaskEventQueue
            {
                public:
                    virtual ~ITaskEventQueue() = default;

                    /// Forwards the next event to the event queue
                    virtual void forward_to_event_queue() = 0;
                    /// Returns the size of the event queue.
                    virtual size_t size() = 0;
                    virtual void register_notification(QueueNotification* notification) = 0;
            };
        }
    }
}

//
// Created by permal on 6/27/17.
//

#pragma once

#include "TaskEventQueue.h"
#include "Link.h"

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            /// In addition to the functionality of the TaskEventQueue<T>, the  SubscribingEventQueue<T> also subscribes
            /// to messages/events sent via the Publisher<T>.
            /// \tparam T The type of event to receive.
            template<typename T>
            class SubscribingTaskEventQueue
                    : public Link<T>, TaskEventQueue<T>
            {
                public:
                    /// Constructor
                    /// \param name The name of the event queue, mainly used for debugging and logging.
                    /// \param size The size of the queue, i.e. the number of items it can hold.
                    /// \param task The Task to which to signal when an event is available.
                    /// \param listener The receiver of the events. Normally this is the same as the task, but it can be
                    /// any object instance.
                    SubscribingTaskEventQueue(const std::string& name, int size, Task& task, IEventListener<T>& listener)
                            :
                            Link<T>(),
                            TaskEventQueue<T>(name, size, task, listener)
                    {
                        this->subscribe(&this->queue);
                    }

                    /// Destructor
                    ~SubscribingTaskEventQueue()
                    {
                        this->unsubscribe(&this->queue);
                    }
            };
        }
    }
}
//
// Created by permal on 6/27/17.
//

#pragma once

#include "TaskEventQueue.h"
#include "Link.h"
#include "ILinkSubscriber.h"

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
                    : TaskEventQueue<T>,
                      ILinkSubscriber<T>
            {
                public:
                    /// Constructor
                    /// \param name The name of the event queue, mainly used for debugging and logging.
                    /// \param size The size of the queue, i.e. the number of items it can hold.
                    /// \param task The Task to which to signal when an event is available.
                    /// \param event_listener The receiver of the events. Normally this is the same as the task, but it can be
                    /// any object instance.
                    SubscribingTaskEventQueue(const std::string& name, size_t size, Task& receive_task, IEventListener<T>& event_listener)
                            :
                            TaskEventQueue<T>(name, size, receive_task, event_listener),
                            link()
                    {
                        link.subscribe(this);
                    }

                    /// Destructor
                    ~SubscribingTaskEventQueue()
                    {
                        link.unsubscribe(this);
                    }

                    bool receive_published_data(const T& data)
                    {
                        return this->push(data);
                    }

                private:
                    Link<T> link;
            };
        }
    }
}
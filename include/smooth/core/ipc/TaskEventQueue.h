//
// Created by permal on 6/27/17.
//

#pragma once

#include <smooth/core/Task.h>
#include "ITaskEventQueue.h"
#include "IEventListener.h"
#include "Link.h"
#include "QueueNotification.h"

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            /// TaskEventQueue expands the functionality of the Queue<T> by, together with the Task, adding the ability
            /// to signal a Task when an item is available, making polling a queue unnecessary which frees up the task
            /// to do other things.
            /// \tparam T The type of events to receive.
            template<typename T>
            class TaskEventQueue : public ITaskEventQueue
            {
                public:
                    friend core::Task;

                    static_assert(std::is_default_constructible<T>::value, "DataType must be default-constructible");
                    static_assert(std::is_assignable<T,T>::value, "DataType must be a assignable");

                    /// Constructor
                    /// \param name The name of the event queue, mainly used for debugging and logging.
                    /// \param size The size of the queue, i.e. the number of items it can hold.
                    /// \param task The Task to which to signal when an event is available.
                    /// \param listener The receiver of the events. Normally this is the same as the task, but it can be
                    /// any object instance.
                    TaskEventQueue(const std::string& name, int size, Task& task, IEventListener<T>& listener)
                            :
                            queue(name + std::string("-TaskEventQueue"), size),
                            task(task),
                            listener(listener)
                    {
                        task.register_queue_with_task(this);
                    }

                    /// Pushes an item into the queue
                    /// \param item The item of which a copy will be placed on the queue.
                    /// \return true if the queue could accept the item, otherwise false.
                    bool push(const T& item)
                    {
                        auto res = queue.push(item);
                        if(res)
                        {
                            notification->notify(this);
                        }
                        return res;
                    }

                    /// Gets the size of the queue.
                    /// \return number of items the queue can hold.
                    int size()
                    {
                        return queue.size();
                    }

                    /// Returns the number of items waiting to be popped.
                    /// \return The number of items in the queue.
                    int count()
                    {
                        return queue.count();
                    }

                    void register_notification(QueueNotification* notification) override
                    {
                        this->notification = notification;
                    }

                protected:
                    Queue<T> queue;
                    QueueNotification* notification = nullptr;
                private:
                    void forward_to_event_queue()
                    {
                        // All messages passed via a queue needs a default constructor
                        // and must be copyable and have the assignment operator.
                        T m;
                        if (queue.pop(m))
                        {
                            listener.event(m);
                        }
                    }

                    Task& task;
                    IEventListener<T>& listener;
            };
        }
    }
}
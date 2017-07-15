//
// Created by permal on 6/27/17.
//

#pragma once

#include <smooth/Task.h>
#include "ITaskEventQueue.h"
#include "IEventListener.h"
#include "Link.h"

namespace smooth
{
    namespace ipc
    {
        template<typename T>
        class TaskEventQueue
                : public Link<T>, public Queue<T>, public ITaskEventQueue
        {
            public:
                TaskEventQueue(const std::string& name, int size, Task& task, IEventListener <T>& listener)
                        :
                        Link<T>(),
                        Queue<T>(name + std::string("-TaskEventQueue"), size),
                        task(task),
                        listener(listener)
                {
                    this->subscribe(this);
                    task.register_queue_with_task(this);
                }


                ~TaskEventQueue()
                {
                    this->unsubscribe(this);
                }

                void forward_to_task()
                {
                    // All messages passed via a queue needs a default constructor and must be copyable.
                    T m;
                    if (Queue<T>::pop(m))
                    {
                        listener.message(m);
                    }
                }

                bool push(const T& item) override
                {
                    return Queue<T>::push(item);
                }

                int get_size() override
                {
                    return Queue<T>::get_size();
                }

                QueueHandle_t get_handle() override
                {
                    return Queue<T>::get_handle();
                }

            private:
                Task& task;
                IEventListener <T>& listener;
        };
    }
}
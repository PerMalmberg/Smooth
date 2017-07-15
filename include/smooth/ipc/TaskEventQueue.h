//
// Created by permal on 6/27/17.
//

#pragma once

#include <smooth/Task.h>
#include "ITaskEventQueue.h"
#include "IEventMessage.h"
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
                    task.report_queue_size(size);
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
                    bool res = Queue<T>::push(item);
                    if (res)
                    {
                        task.message_available(this);
                    }

                    return res;
                }

            private:
                Task& task;
                IEventListener <T>& listener;
        };
    }
}
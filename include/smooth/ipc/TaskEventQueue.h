//
// Created by permal on 6/27/17.
//

#pragma once

#include "EventQueue.h"
#include <smooth/Task.h>
#include "ITaskEventQueue.h"

namespace smooth
{
    namespace ipc
    {
        template<typename T>
        class TaskEventQueue
                : public EventQueue<T>, public ITaskEventQueue
        {
            public:
                TaskEventQueue(const std::string& name, int size, Task& task, IEventListener<T>& listener)
                        : EventQueue<T>(std::string("TaskEventQueue") + name, size), task(task), listener(listener)
                {
                }


                void forward_to_task()
                {
                    T m;
                    if (this->pop(m))
                    {
                        listener.message(m);
                    }
                }


            protected:
                void push(const T& item) override
                {
                    EventQueue<T>::push(item);
                    task.message_available(this);
                }

            private:
                Task& task;
                IEventListener<T>& listener;
        };
    }
}
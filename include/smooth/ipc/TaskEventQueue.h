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
                    task.register_event_queue(this);
                }


                void pop_next()
                {
                    T m;
                    if (this->pop(m))
                    {
                        m.execute(listener);
                    }
                }


            protected:
                void push(const T& item) override
                {
                    EventQueue<T>::push(item);
                    task.message_available();
                }

            private:
                Task& task;
                IEventListener<T>& listener;
        };
    }
}
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
            template<typename T>
            class SubscribingTaskEventQueue
                    : public Link<T>, TaskEventQueue<T>
            {
                public:
                    SubscribingTaskEventQueue(const std::string& name, int size, Task& task, IEventListener<T>& listener)
                            :
                            Link<T>(),
                            TaskEventQueue<T>(name, size, task, listener)
                    {
                        this->subscribe(&this->queue);
                    }


                    ~SubscribingTaskEventQueue()
                    {
                        this->unsubscribe(&this->queue);
                    }
            };
        }
    }
}
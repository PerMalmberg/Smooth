//
// Created by permal on 6/26/17.
//

#pragma once

#include "Queue.h"

namespace smooth
{
    namespace ipc
    {
        template<typename T>
        class Link;

        template<typename T>
        class EventQueue
        {
            public:
                EventQueue(const std::string& name, int size)
                        : queue(std::string("SubscribingEventQueue-") + name, size)
                {
                    link.subscribe(this);
                }

                virtual ~EventQueue()
                {
                    link.unsubscribe(this);
                }

                bool pop(T& t, std::chrono::milliseconds wait_time)
                {
                    return queue.pop(t, wait_time);
                }


            private:
                Queue<T> queue;
                Link<T> link;

                void enqueue(T& item)
                {
                    queue.push(item);
                }

                friend class Link<T>;
        };
    }
}

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
                        : queue(std::string("EventQueue-") + name, size)
                {
                    link.subscribe(this);
                }

                virtual ~EventQueue()
                {
                    link.unsubscribe(this);
                }

                bool pop(T& t)
                {
                    // Returns immediately if queue is empty
                    return queue.pop(t);
                }

                bool pop(T& t, std::chrono::milliseconds wait_time)
                {
                    return queue.pop(t, wait_time);
                }

            protected:
                virtual void push(const T& item)
                {
                    queue.push(item);
                }

            private:
                Queue<T> queue;
                Link<T> link;

                friend class Link<T>;
        };
    }
}

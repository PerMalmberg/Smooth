//
// Created by permal on 6/26/17.
//

#pragma once

#include <forward_list>
#include <chrono>
#include "EventQueue.h"
#include "Mutex.h"

namespace smooth
{
    namespace ipc
    {
        template<typename T>
        class Link
        {
            public:
                Link()
                {
                }

                Link(const Link&) = delete;

                virtual ~Link()
                {
                    get_subscribers().clear();
                }

                void subscribe(EventQueue<T>* sub)
                {
                    Mutex::Lock l(get_mutex());
                    get_subscribers().push_front(sub);
                }

                void unsubscribe(EventQueue<T>* sub)
                {
                    Mutex::Lock l(get_mutex());
                    get_subscribers().remove(sub);
                }

                static void publish(T& item)
                {
                    // Wait for semaphore to become available
                    Mutex::Lock l(get_mutex());

                    for (auto subscriber : get_subscribers())
                    {
                        subscriber->enqueue(item);
                    }
                }

                static std::forward_list<EventQueue<T>*>& get_subscribers()
                {
                    // Place list in method to ensure linker finds it.
                    static std::forward_list<EventQueue<T>*> subscribers;
                    return subscribers;
                }

            private:
                static Mutex& get_mutex()
                {
                    static Mutex m;
                    return m;
                }
        };
    }
}
//
// Created by permal on 6/26/17.
//

#pragma once

#include <forward_list>
#include <chrono>
#include "SubscribingEventQueue.h"
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

                void subscribe(SubscribingEventQueue<T>* sub)
                {
                    get_subscribers().push_front(sub);
                }

                void unsubscribe(SubscribingEventQueue<T>* sub)
                {
                    get_subscribers().remove(sub);
                }

                static void publish(T& item)
                {
                   static Mutex m;

                    // Try until we succeed in sending the message
                    bool done = false;
                    do
                    {
                        // Wait for semaphore to become available
                        done = m.acquire();
                        if (done)
                        {
                            for (auto subscriber : get_subscribers())
                            {
                                subscriber->enqueue(item);
                            }

                            m.release();
                        }
                    }
                    while (!done);
                }


                static std::forward_list<SubscribingEventQueue<T>*>& get_subscribers()
                {
                    // Place list in method to ensure linker finds it.
                    static std::forward_list<SubscribingEventQueue<T>*> subscribers;
                    return subscribers;
                }

            private:

        };
    }
}
//
// Created by permal on 6/26/17.
//

#pragma once

#include <forward_list>
#include <chrono>
#include "Mutex.h"
#include "Queue.h"

namespace smooth
{
    namespace ipc
    {
        template<typename T>
        class Link
        {
            public:
                Link() = default;

                Link(const Link&) = delete;

                virtual ~Link();

                void subscribe(Queue<T>* sub);

                void unsubscribe(Queue<T>* sub);

                static std::forward_list<Queue<T>*>& get_subscribers();

                static bool publish(T& item)
                {
                    Mutex::Lock l(get_mutex());
                    bool res = false;

                    for (Queue<T>* subscriber : get_subscribers())
                    {
                        res &= subscriber->push(item);
                    }

                    return res;
                }

            private:
                static Mutex& get_mutex()
                {
                    static Mutex m;
                    return m;
                }
        };


        template<typename T>
        Link<T>::~Link()
        {
            // Do not clear subscribers - it breaks all subscriptions when an
            // instance of a class inheriting from Link<> is destructed.
            // The correct way is to unsubscribe in the subclass destructor.
        }

        template<typename T>
        void Link<T>::subscribe(Queue<T>* sub)
        {
            Mutex::Lock l(get_mutex());
            get_subscribers().push_front(sub);
        }

        template<typename T>
        void Link<T>::unsubscribe(Queue<T>* sub)
        {
            Mutex::Lock l(get_mutex());
            get_subscribers().remove(sub);
        }

        template<typename T>
        std::forward_list<Queue<T>*>& Link<T>::get_subscribers()
        {
            // Place list in method to ensure linker finds it.
            static std::forward_list<Queue<T>*> subscribers;
            return subscribers;
        }
    }
}
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
    namespace core
    {
        namespace ipc
        {
            /// The Link class is used to bind subscribers of a certain message type together in a thread-safe manner
            /// so that any Task can call core::ipc::Publisher<T>::publish(T&) to distribute a copy of an event
            /// to each subscriber.
            /// \tparam T The type of event to distribute.
            template<typename T>
            class Link
            {
                public:
                    Link() = default;

                    Link(const Link&) = delete;

                    virtual ~Link();

                    /// Subscribe to messages
                    /// \param queue The queue which shall receive the messages.
                    void subscribe(Queue<T>* queue);

                    /// Unsubscribes to messages
                    /// \param queue
                    void unsubscribe(Queue<T>* queue);

                    /// Publishes a copy of the the provided item to each subscriber.
                    /// \param item The item to publish
                    /// \return true of all subscribers could receive the item, false if one or more queues were full.
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

                    static std::forward_list<Queue<T>*>& get_subscribers();
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
            void Link<T>::subscribe(Queue<T>* queue)
            {
                Mutex::Lock l(get_mutex());
                get_subscribers().push_front(queue);
            }

            template<typename T>
            void Link<T>::unsubscribe(Queue<T>* queue)
            {
                Mutex::Lock l(get_mutex());
                get_subscribers().remove(queue);
            }

            template<typename T>
            std::forward_list<Queue<T>*>& Link<T>::get_subscribers()
            {
                // Place list in method to ensure linker finds it, it also guarantees
                // no race condition exists while constructing the forward_list.
                static std::forward_list<Queue<T>*> subscribers;
                return subscribers;
            }
        }
    }
}
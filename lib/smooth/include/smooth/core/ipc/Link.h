/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <forward_list>
#include <chrono>
#include <mutex>
#include "Queue.h"
#include "ILinkSubscriber.h"

namespace smooth::core::ipc
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

            // Do not clear subscribers on sdestruction - it breaks all
            // subscriptions when an instance of a class inheriting from
            // Link<> is destructed. The correct way is to unsubscribe
            // in the subclass destructor.
            virtual ~Link() = default;

            /// Subscribe to messages
            /// \param queue The subscriber which shall receive the messages.
            void subscribe(ILinkSubscriber<T>* subscriber);

            /// Unsubscribes to messages
            /// \param queue
            void unsubscribe(ILinkSubscriber<T>* subscriber);

            /// Publishes a copy of the the provided item to each subscriber.
            /// \param item The item to publish
            /// \return true of all subscribers could receive the item, false if one or more queues were full.
            static bool publish(const T& item)
            {
                std::lock_guard<std::mutex> l(get_mutex());
                bool res = false;

                for (auto subscriber : get_subscribers())
                {
                    res &= subscriber->receive_published_data(item);
                }

                return res;
            }

        private:
            static std::forward_list<ILinkSubscriber<T>*>& get_subscribers();

            static std::mutex& get_mutex()
            {
                static std::mutex m;

                return m;
            }
    };

    template<typename T>
    void Link<T>::subscribe(ILinkSubscriber<T>* subscriber)
    {
        std::lock_guard<std::mutex> l(get_mutex());
        get_subscribers().push_front(subscriber);
    }

    template<typename T>
    void Link<T>::unsubscribe(ILinkSubscriber<T>* subscriber)
    {
        std::lock_guard<std::mutex> l(get_mutex());
        get_subscribers().remove(subscriber);
    }

    template<typename T>
    std::forward_list<ILinkSubscriber<T>*>& Link<T>::get_subscribers()
    {
        // Place list in method to ensure linker finds it, it also guarantees
        // no race condition exists while constructing the forward_list.
        static std::forward_list<ILinkSubscriber<T>*> subscribers;

        return subscribers;
    }
}

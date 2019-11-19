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

#include <chrono>
#include <string>
#include <vector>
#include <mutex>
#include <algorithm>
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::core::ipc
{
    /// T Queue<T> is precisely what that name suggest - a queue that holds items of type T.
    /// It is also thread-safe. It can be used either as a stand alone queue or as the base for
    /// more specialized implementations, such as the TaskEventQueue and SubscribingTaskEventQueue.
    /// Please note that this implementation supports actual C++ objects as opposed to the FreeRTOS
    /// plain data-only queues. This means that you can place any type of C++ object on these queues
    /// as long as the objects are copyable (the default copy constructor and assignment operator are enough)
    /// Items are placed on the queue by copy, not by reference.
    /// \tparam T The type of object to hold in the queue.
    template<typename T>
    class Queue
    {
        public:
            /// Constructor
            /// \param name The name of the queue, mainly used for debugging and logging.
            /// \param size The size of the queue, i.e. the number of items it can hold.
            explicit Queue(int size)
                    : queue_size(size),
                      items(),
                      guard()
            {
                items.reserve(static_cast<size_t>(size));
            }

            /// Destructor
            virtual ~Queue()
            {
                std::lock_guard<std::mutex> lock(guard);
                items.clear();
            }

            /// Gets the size of the queue.
            /// \return number of items the queue can hold.
            int size()
            {
                std::lock_guard<std::mutex> lock(guard);

                return queue_size;
            }

            /// Pushes an item into the queue
            /// \param item The item of which a copy will be placed on the queue.
            /// \return true if the queue could accept the item, otherwise false.
            bool push(const T& item)
            {
                std::lock_guard<std::mutex> lock(guard);

                bool res = items.size() < static_cast<size_t>(queue_size);

                if (res)
                {
                    items.emplace_back(item);
                }

                return res;
            }

            /// Pops an item off the queue.
            /// \param target A reference to an instance of T which will be assigned the item taken from the queue.
            /// \return true if an item could be received, otherwise false.
            bool pop(T& target)
            {
                std::lock_guard<std::mutex> lock(guard);

                bool res = items.size() > 0;

                if (res)
                {
                    target = items.front();
                    items.erase(items.begin());
                }

                return res;
            }

            /// Returns a value indicating if the queue is empty.
            /// \return true if empty, otherwise false.
            bool empty()
            {
                return count() == 0;
            }

            /// Returns the number of items waiting to be popped.
            /// \return The number of items in the queue.
            int count()
            {
                std::lock_guard<std::mutex> lock(guard);

                return static_cast<int>(items.size());
            }

        private:
            const int queue_size;
            std::vector<T> items;
            std::mutex guard;
    };
}

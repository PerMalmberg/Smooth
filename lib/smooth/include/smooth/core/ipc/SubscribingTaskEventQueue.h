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

#include "TaskEventQueue.h"
#include "Link.h"
#include "ILinkSubscriber.h"
#include "smooth/core/util/create_protected.h"

namespace smooth::core::ipc
{
    /// In addition to the functionality of the TaskEventQueue<T>, the  SubscribingEventQueue<T> also subscribes
    /// to messages/events sent via the Publisher<T>.
    /// \tparam T The type of event to receive.
    template<typename T>
    class SubscribingTaskEventQueue
        : public TaskEventQueue<T>
    {
        public:
            /// Destructor
            ~SubscribingTaskEventQueue()
            {
                link.unsubscribe(&wrapper);
            }

            SubscribingTaskEventQueue() = delete;

            SubscribingTaskEventQueue(const SubscribingTaskEventQueue&) = delete;

            SubscribingTaskEventQueue(SubscribingTaskEventQueue&&) = delete;

            SubscribingTaskEventQueue& operator=(const SubscribingTaskEventQueue&) = delete;

            SubscribingTaskEventQueue& operator=(const SubscribingTaskEventQueue&&) = delete;

            bool push(const T& item) override
            {
                return this->push_internal(item, this->template shared_from_base<SubscribingTaskEventQueue<T>>());
            }

            static auto create(int size, Task& task, IEventListener<T>& listener)
            {
                auto queue = smooth::core::util::create_protected_shared<SubscribingTaskEventQueue<T>>(size, task,
                                                                                                       listener);
                queue->link_up();

                return queue;
            }

        protected:
            /// Constructor
            /// \param name The name of the event queue, mainly used for debugging and logging.
            /// \param size The size of the queue, i.e. the number of items it can hold.
            /// \param task The Task to which to signal when an event is available.
            /// \param listener The receiver of the events. Normally this is the same as the task, but it can be
            /// any object instance.
            SubscribingTaskEventQueue(int size, Task& task, IEventListener<T>& listener)
                    :
                      TaskEventQueue<T>(size, task, listener),
                      link()
            {
            }

        private:
            void link_up()
            {
                wrapper = LinkWrapper{ this->template shared_from_base<SubscribingTaskEventQueue<T>>() };
                link.subscribe(&wrapper);
            }

            class LinkWrapper : public ILinkSubscriber<T>
            {
                public:
                    explicit LinkWrapper(std::weak_ptr<SubscribingTaskEventQueue<T>> queue)
                            : queue(std::move(queue))
                    {
                    }

                    LinkWrapper() = default;

                    LinkWrapper(const LinkWrapper&) = delete;

                    LinkWrapper(LinkWrapper&&) = delete;

                    LinkWrapper& operator=(const LinkWrapper&) = delete;

                    LinkWrapper& operator=(LinkWrapper&&) noexcept = default;

                    ~LinkWrapper() = default;

                    bool receive_published_data(const T& data) override
                    {
                        bool res = true;
                        auto q = queue.lock();

                        if (q)
                        {
                            res = q->push(data);
                        }

                        return res;
                    }

                private:
                    std::weak_ptr<SubscribingTaskEventQueue<T>> queue;
            };

            Link<T> link;
            LinkWrapper wrapper;
    };
}

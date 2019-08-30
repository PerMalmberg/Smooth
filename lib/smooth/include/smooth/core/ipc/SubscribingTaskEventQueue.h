// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "TaskEventQueue.h"
#include "Link.h"
#include "ILinkSubscriber.h"
#include <smooth/core/util/create_protected.h>

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

            static auto create(const std::string& name, int size, Task& task, IEventListener<T>& listener)
            {
                auto queue = smooth::core::util::create_protected_shared<SubscribingTaskEventQueue<T>>(name, size, task,
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
            SubscribingTaskEventQueue(const std::string& name, int size, Task& task, IEventListener<T>& listener)
                    :
                      TaskEventQueue<T>(name, size, task, listener),
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

                    LinkWrapper& operator=(LinkWrapper&&) = default;

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

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

#include <smooth/core/Task.h>
#include <memory>
#include "ITaskEventQueue.h"
#include "IEventListener.h"
#include "QueueNotification.h"
#include <smooth/core/util/create_protected.h>

namespace smooth::core::ipc
{
    /// TaskEventQueue expands the functionality of the Queue<T> by, together with the Task, adding the ability
    /// to signal a Task when an item is available, making polling a queue unnecessary which frees up the task
    /// to do other things.
    /// \tparam T The type of events to receive.
    template<typename T>
    class TaskEventQueue
        : public ITaskEventQueue,
        public std::enable_shared_from_this<TaskEventQueue<T>>
    {
        public:
            friend core::Task;

            static_assert(std::is_default_constructible<T>::value, "DataType must be default-constructible");
            static_assert(std::is_assignable<T, T>::value, "DataType must be a assignable");

            static auto create(const std::string& name, int size, Task& owner_task, IEventListener<T>& event_listener)
            {
                return smooth::core::util::create_protected_shared<TaskEventQueue<T>>(name, size, owner_task,
                                                                                      event_listener);
            }

            ~TaskEventQueue() override
            {
                notif->remove_expired_queues();
            }

            TaskEventQueue() = delete;

            TaskEventQueue(const TaskEventQueue&) = delete;

            TaskEventQueue(TaskEventQueue&&) = delete;

            TaskEventQueue& operator=(const TaskEventQueue&) = delete;

            TaskEventQueue& operator=(const TaskEventQueue&&) = delete;

            /// Pushes an item into the queue
            /// \param item The item of which a copy will be placed on the queue.
            /// \return true if the queue could accept the item, otherwise false.
            virtual bool push(const T& item)
            {
                return push_internal(item, this->shared_from_this());
            }

            /// Gets the size of the queue.
            /// \return number of items the queue can hold.
            int size() override
            {
                return queue.size();
            }

            /// Returns the number of items waiting to be popped.
            /// \return The number of items in the queue.
            int count()
            {
                return queue.count();
            }

            void register_notification(QueueNotification* notification) override
            {
                notif = notification;
            }

            void clear()
            {
                while (!queue.empty())
                {
                    T t;
                    queue.pop(t);
                }
            }

        protected:
            /// Constructor
            /// \param name The name of the event queue, mainly used for debugging and logging.
            /// \param size The size of the queue, i.e. the number of items it can hold.
            /// \param task The Task to which to signal when an event is available.
            /// \param listener The receiver of the events. Normally this is the same as the task, but it can be
            /// any object instance.
            TaskEventQueue(const std::string& name, int size, Task& task, IEventListener<T>& listener)
                    :
                      queue(name + std::string("-TaskEventQueue"), size),
                      task(task),
                      listener(listener)
            {
                task.register_queue_with_task(this);
            }

            bool push_internal(const T& item, const std::weak_ptr<ITaskEventQueue>& receiver)
            {
                auto res = queue.push(item);

                if (res)
                {
                    notif->notify(receiver);
                }

                return res;
            }

            template<typename Derived>
            std::shared_ptr<Derived> shared_from_base()
            {
                return std::static_pointer_cast<Derived>(this->shared_from_this());
            }

            Queue<T> queue;
            QueueNotification* notif = nullptr;
        private:
            void forward_to_event_listener() override
            {
                // All messages passed via a queue needs a default constructor
                // and must be copyable and have the assignment operator.
                T m;

                if (queue.pop(m))
                {
                    listener.event(m);
                }
            }

            Task& task;
            IEventListener<T>& listener;
    };
}

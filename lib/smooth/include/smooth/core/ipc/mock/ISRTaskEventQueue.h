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

#include "smooth/core/Task.h"
#include "smooth/core/util/create_protected.h"

namespace smooth::core::ipc
{
    /// ISRTaskEventQueue is similar to TaskEventQueue with two important differences:
    /// - It allows items to be queued from an interrupt context.
    /// - It does *not* support complex C++ objects to be enqueued, i.e. DataType must be a trivial type.
    /// \tparam DataType The type of data to carry on the queue.
    /// \tparam Size The size of the queue.
    template<typename DataType, int Size>
    class ISRTaskEventQueue
        : public IISRTaskEventQueue<DataType>,
        public IPolledTaskQueue,
        public std::enable_shared_from_this<ISRTaskEventQueue<DataType, Size>>
    {
        public:
            friend core::Task;

            static auto create(Task& task, IEventListener<DataType>& listener)
            {
                return smooth::core::util::create_protected_shared<ISRTaskEventQueue<DataType, Size>>(task, listener);
            }

            ~ISRTaskEventQueue() override = default;

            void signal(const DataType&) {}

            int size() override
            {
                return Size;
            }

            void register_notification(QueueNotification* /*notification*/) override
            {
            }

            void poll() override
            {
            }

        protected:
            ISRTaskEventQueue(Task&, IEventListener<DataType>&) {}
        private:
            void forward_to_event_listener() override {}
    };
}

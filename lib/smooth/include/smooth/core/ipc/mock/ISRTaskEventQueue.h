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
#include <smooth/core/util/create_protected.h>

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

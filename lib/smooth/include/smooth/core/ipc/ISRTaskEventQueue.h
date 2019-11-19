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

#include "IISRTaskEventQueue.h"
#include "IPolledTaskQueue.h"
#include <freertos/FreeRTOS.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <freertos/queue.h>
#pragma GCC diagnostic pop
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

            ~ISRTaskEventQueue() override;

            SMOOTH_MEM_ATTR void signal(const DataType& data) override;

            int size() override
            {
                return Size;
            }

            void register_notification(QueueNotification* notif) override
            {
                notification = notif;
            }

            void poll() override
            {
                // Do we have an item in the queue?
                if (notification
                    && read_since_poll
                    && uxQueueMessagesWaiting(queue) > 0)
                {
                    read_since_poll = false;
                    notification->notify(this->shared_from_this());
                }
            }

        protected:
            ISRTaskEventQueue(Task& task, IEventListener<DataType>& listener);

        private:
            void forward_to_event_listener() override;

            QueueHandle_t queue;
            Task& task;
            IEventListener<DataType>& listener;
            QueueNotification* notification = nullptr;
            bool read_since_poll = true;
    };

    template<typename DataType, int Size>
    ISRTaskEventQueue<DataType, Size>::ISRTaskEventQueue(Task& task, IEventListener<DataType>& listener)
            : task(task), listener(listener)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
        queue = xQueueCreate(Size, sizeof(DataType));
#pragma GCC diagnostic pop
        task.register_polled_queue_with_task(this);
    }

    /// \note This method runs in ISR context
    template<typename DataType, int Size>
    void ISRTaskEventQueue<DataType, Size>::signal(const DataType& data)
    {
        // There is a possibility that we loose signals here if the queue is full.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

        while (xQueueSendToBackFromISR(queue, &data, nullptr) == errQUEUE_FULL)
        {
            // Drop oldest message, this way we'll always get the last data value onto the queue
            DataType lost;
            xQueueReceiveFromISR(queue, &lost, nullptr);
        }

#pragma GCC diagnostic pop
    }

    template<typename DataType, int Size>
    void ISRTaskEventQueue<DataType, Size>::forward_to_event_listener()
    {
        // All messages passed via a queue needs a default constructor
        // and must be copyable and have the assignment operator.
        DataType m;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"

        if (xQueueReceive(queue, &m, 1) == pdTRUE)
        {
            listener.event(m);
        }

#pragma GCC diagnostic pop

        read_since_poll = true;
    }

    template<typename DataType, int Size>
    ISRTaskEventQueue<DataType, Size>::~ISRTaskEventQueue()
    {
        task.unregister_polled_queue_with_task(this);
    }
}

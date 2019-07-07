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

#include "IISRTaskEventQueue.h"
#include "IPolledTaskQueue.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <smooth/core/Task.h>

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
              public IPolledTaskQueue
    {

        public:
            friend core::Task;

            ISRTaskEventQueue(Task& task, IEventListener <DataType>& listener);

            ~ISRTaskEventQueue() override;

            IRAM_ATTR void signal(const DataType& data) override;

            int size() override
            {
                return Size;
            }

            void register_notification(QueueNotification* notification) override
            {
                this->notification = notification;
            }

            void poll() override
            {
                // Do we have an item in the queue?
                if (notification
                    && read_since_poll
                    && uxQueueMessagesWaiting(queue) > 0)
                {
                    read_since_poll = false;
                    notification->notify(this);
                }
            }

        private:
            void forward_to_event_listener() override;

            QueueHandle_t queue;
            Task& task;
            IEventListener <DataType>& listener;
            QueueNotification* notification = nullptr;
            bool read_since_poll = true;
    };

    template<typename DataType, int Size>
    ISRTaskEventQueue<DataType, Size>::ISRTaskEventQueue(Task& task, IEventListener <DataType>& listener)
            :task(task), listener(listener)
    {
        queue = xQueueCreate(Size, sizeof(DataType));
        task.register_polled_queue_with_task(this);
    }

    /// \note This method runs in ISR context
    template<typename DataType, int Size>
    void ISRTaskEventQueue<DataType, Size>::signal(const DataType& data)
    {
        // There is a possibility that we loose signals here if the queue is full.
        while (xQueueSendToBackFromISR(queue, &data, nullptr) == errQUEUE_FULL)
        {
            // Drop oldest message, this way we'll always get the last data value onto the queue
            DataType lost;
            xQueueReceiveFromISR(queue, &lost, nullptr);
        }
    }

    template<typename DataType, int Size>
    void ISRTaskEventQueue<DataType, Size>::forward_to_event_listener()
    {
        // All messages passed via a queue needs a default constructor
        // and must be copyable and have the assignment operator.
        DataType m;

        if (xQueueReceive(queue, &m, 1) == pdTRUE)
        {
            listener.event(m);
        }

        read_since_poll = true;
    }

    template<typename DataType, int Size>
    ISRTaskEventQueue<DataType, Size>::~ISRTaskEventQueue()
    {
        task.unregister_polled_queue_with_task(this);
    }
}

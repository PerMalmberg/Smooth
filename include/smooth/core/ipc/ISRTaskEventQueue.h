//
// Created by permal on 9/3/17.
//

#pragma once

#include "IISRTaskEventQueue.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <smooth/core/Task.h>

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            /// ISRTaskEventQueue is similar to TaskEventQueue with two important differences:
            /// - It allows items to be queued from an interrupt context.
            /// - It does *not* support complex C++ objects to be enqueued, i.e. DataType must be a trivial type.
            /// \tparam DataType The type of data to carry on the queue.
            /// \tparam Size The size of the queue.
            template<typename DataType, int Size>
            class ISRTaskEventQueue
                    : public IISRTaskEventQueue<DataType>,
                      public ITaskEventQueue
            {

                public:
                    friend core::Task;

                    ISRTaskEventQueue(Task& task, IEventListener<DataType>& listener);

                    IRAM_ATTR void signal(const DataType& data) override;

                    int size() override
                    {
                        return Size;
                    }

                    void register_notification(QueueNotification* notification) override
                    {
                        this->notification = notification;
                    }

                private:
                    void forward_to_event_queue();

                    QueueHandle_t queue;
                    Task& task;
                    IEventListener<DataType>& listener;
                    QueueNotification* notification = nullptr;
            };

            template<typename DataType, int Size>
            ISRTaskEventQueue<DataType,Size>::ISRTaskEventQueue(Task& task, IEventListener<DataType>& listener)
                    :task(task), listener(listener)
            {
                queue = xQueueCreate(Size, sizeof(DataType));
                task.register_queue_with_task(this);
            }

            template<typename DataType, int Size>
            void ISRTaskEventQueue<DataType,Size>::signal(const DataType& data)
            {
                // There is a possibility that we loose signals here if the queue is full.
                if(pdTRUE == xQueueSendToBackFromISR(queue, &data, nullptr))
                {
                    if(notification)
                    {
                        notification->notify(this);
                    }
                }
            }

            template<typename DataType, int Size>
            void ISRTaskEventQueue<DataType,Size>::forward_to_event_queue()
            {
                // All messages passed via a queue needs a default constructor
                // and must be copyable and have the assignment operator.
                DataType m;

                if (xQueueReceiveFromISR(queue, &m, nullptr) == pdTRUE)
                {
                    listener.event(m);
                }
            }
        }
    }
}

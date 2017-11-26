//
// Created by permal on 9/3/17.
//

#pragma once

#include "IISRTaskEventQueue.h"
#include "IPolledTaskQueue.h"
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
                      public IPolledTaskQueue
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

                    void poll() override
                    {
                        DataType t;
                        if (forward_performed
                            && notification
                            && xQueuePeek(queue, &t, 0) == pdTRUE)
                        {
                            notification->notify(this);
                            forward_performed = false;
                        }
                    }

                private:
                    void forward_to_event_queue();

                    QueueHandle_t queue;
                    Task& task;
                    IEventListener<DataType>& listener;
                    QueueNotification* notification = nullptr;
                    bool forward_performed = true;
            };

            template<typename DataType, int Size>
            ISRTaskEventQueue<DataType, Size>::ISRTaskEventQueue(Task& task, IEventListener<DataType>& listener)
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
                xQueueSendToBackFromISR(queue, &data, nullptr);
            }

            template<typename DataType, int Size>
            void ISRTaskEventQueue<DataType, Size>::forward_to_event_queue()
            {
                // All messages passed via a queue needs a default constructor
                // and must be copyable and have the assignment operator.
                DataType m;

                if (xQueueReceive(queue, &m, 1) == pdTRUE)
                {
                    listener.event(m);
                }

                forward_performed = true;
            }
        }
    }
}

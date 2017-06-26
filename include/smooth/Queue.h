//
// Created by permal on 6/25/17.
//
#pragma once

#include <freertos/queue.h>
#include <esp_log.h>
#include <chrono>

namespace smooth
{
    template<typename T, int size>
    class Queue
    {
        public:
            Queue()
            {
                handle = xQueueCreate(size, sizeof(T));
                if (handle == nullptr)
                {
                    ESP_LOGE("Queue", "Failed to create Queue");
                    abort();
                }
            }

            virtual ~Queue()
            {
                if (handle != nullptr)
                {
                    vQueueDelete(handle);
                }
            }

            bool push(T& item, std::chrono::milliseconds wait_time = std::chrono::milliseconds(0))
            {
                auto res = xQueueSend(handle, &item, to_ticks(wait_time));
                return res == pdTRUE;
            }

            bool push_from_isr(T& item)
            {
                auto res = xQueueSendFromISR(handle, &item, nullptr);
                return res == pdTRUE;
            }

            bool pop(T& target, std::chrono::milliseconds wait_time = std::chrono::milliseconds(0))
            {
                auto res = xQueueReceive(handle, &target, to_ticks(wait_time));
                return res == pdTRUE;
            }

            bool pop_from_isr(T& target)
            {
                auto res = xQueueReceiveFromISR(handle, &target, nullptr);
                return res == pdTRUE;
            }

            bool empty()
            {
                UBaseType_t count = uxQueueMessagesWaiting(handle);
                return count == 0;
            }

            bool empty_from_isr()
            {
                UBaseType_t count = uxQueueMessagesWaitingFromISR(handle);
                return count == 0;
            }

        private:
            QueueHandle_t handle = nullptr;

            TickType_t to_ticks(std::chrono::milliseconds ms)
            {
                return ms.count() / portTICK_PERIOD_MS;
            }
    };
}

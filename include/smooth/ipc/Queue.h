//
// Created by permal on 6/25/17.
//
#pragma once

#include <freertos/queue.h>
#include <esp_log.h>
#include <chrono>
#include <string>

namespace smooth
{
    namespace ipc
    {
        template<typename T>
        class Queue
        {
            public:
                Queue(const std::string& name, int size)
                        : name(name),
                          queue_size(size)
                {
                    ESP_LOGV("Queue", "Creating queue '%s', with %d items of size %d.", name.c_str(), size, sizeof(T));
                    handle = xQueueCreate(size, sizeof(T));
                    if (handle == nullptr)
                    {
                        ESP_LOGE("Queue", "Failed to create Queue '%s'", name.c_str());
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

                void set_size(int size)
                {
                    if (handle != nullptr && size > queue_size && size > 0)
                    {
                        ESP_LOGV("Queue", "Resizing queue '%s', from %d items to %d items of size %d.", name.c_str(),
                                 queue_size, size, sizeof(T));
                        vQueueDelete(handle);
                        handle = xQueueCreate(size, sizeof(T));
                        queue_size = size;
                    }
                }

                int get_size()
                {
                    return queue_size;
                }

                QueueHandle_t get_handle() const
                {
                    return handle;
                }

                virtual bool push(const T& item)
                {
                    bool res = xQueueSend(handle, &item, 0) == pdTRUE;
                    if (!res)
                    {
                        ESP_LOGE("Queue", "'%s': Could not push", name.c_str());
                    }
                    return res;
                }

                virtual bool push(const T& item, std::chrono::milliseconds wait_time)
                {
                    return xQueueSend(handle, &item, to_ticks(wait_time)) == pdTRUE;
                }

                bool push_from_isr(const T& item)
                {
                    return xQueueSendFromISR(handle, &item, nullptr) == pdTRUE;
                }

                bool pop(T& target)
                {
                    return xQueueReceive(handle, &target, 0);
                }

                bool pop(T& target, std::chrono::milliseconds wait_time)
                {
                    return xQueueReceive(handle, &target, to_ticks(wait_time)) == pdTRUE;
                }

                bool pop_from_isr(T& target)
                {
                    return xQueueReceiveFromISR(handle, &target, nullptr) == pdTRUE;
                }

                bool empty()
                {
                    return uxQueueMessagesWaiting(handle) == 0;
                }

                bool empty_from_isr()
                {
                    return uxQueueMessagesWaitingFromISR(handle) == 0;
                }

            private:
                QueueHandle_t handle = nullptr;
                std::string name;
                int queue_size;

                TickType_t to_ticks(std::chrono::milliseconds ms)
                {
                    return pdMS_TO_TICKS(ms.count());
                }
        };
    }
}
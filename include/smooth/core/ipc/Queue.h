//
// Created by permal on 6/25/17.
//
#pragma once

#include <freertos/queue.h>
#include <esp_log.h>
#include <esp_attr.h>
#include <chrono>
#include <string>
#include <vector>
#include <smooth/core/ipc/Mutex.h>

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            template<typename T>
            class Queue
            {
                public:
                    Queue(const std::string& name, int size)
                            : name(name),
                              queue_size(size),
                              items(),
                              guard()
                    {
                        ESP_LOGV("Queue", "Creating queue '%s', with %d items of size %d.", name.c_str(), size,
                                 sizeof(T));

                        handle = xQueueCreate(size, sizeof(uint8_t));
                        if (handle == nullptr)
                        {
                            ESP_LOGE("Queue", "Failed to create Queue '%s'", name.c_str());
                            abort();
                        }

                        items.reserve(size);
                    }

                    virtual ~Queue()
                    {
                        smooth::core::ipc::Mutex::Lock lock(guard);
                        if (handle != nullptr)
                        {
                            vQueueDelete(handle);
                        }
                        items.clear();
                    }

                    void set_size(int size)
                    {
                        smooth::core::ipc::Mutex::Lock lock(guard);
                        if (handle != nullptr && size > queue_size && size > 0)
                        {
                            ESP_LOGV("Queue", "Resizing queue '%s', from %d items to %d items of size %d.",
                                     name.c_str(),
                                     queue_size, size, sizeof(T));
                            vQueueDelete(handle);
                            handle = xQueueCreate(size, sizeof(uint8_t));
                            queue_size = size;
                            items.reserve(size);
                        }
                    }

                    int size()
                    {
                        smooth::core::ipc::Mutex::Lock lock(guard);
                        return queue_size;
                    }

                    QueueHandle_t get_handle() const
                    {
                        return handle;
                    }

                    bool push(const T& item)
                    {
                        return push(item, std::chrono::milliseconds(0));
                    }

                    bool push(const T& item, std::chrono::milliseconds wait_time)
                    {
                        smooth::core::ipc::Mutex::Lock lock(guard);

                        uint8_t dummy = 0;
                        bool res = xQueueSend(handle, &dummy, to_ticks(wait_time)) == pdTRUE;

                        if (res)
                        {
                            items.push_back(item);
                        }

                        return res;
                    }

                    bool pop(T& target)
                    {
                        return pop(target, std::chrono::milliseconds(0));
                    }

                    bool pop(T& target, std::chrono::milliseconds wait_time)
                    {
                        smooth::core::ipc::Mutex::Lock lock(guard);
                        uint8_t dummy = 0;
                        bool res = xQueueReceive(handle, &dummy, 0) == pdTRUE;
                        if( res )
                        {
                            target = items.front();
                            items.erase(items.begin());
                        }

                        return res;
                    }

                    bool empty()
                    {
                        return count() == 0;
                    }

                    int count()
                    {
                        smooth::core::ipc::Mutex::Lock lock(guard);
                        return uxQueueMessagesWaiting(handle);
                    }

                private:
                    QueueHandle_t handle = nullptr;
                    std::string name;
                    int queue_size;
                    std::vector<T> items;
                    smooth::core::ipc::Mutex guard;

                    TickType_t to_ticks(std::chrono::milliseconds ms)
                    {
                        return pdMS_TO_TICKS(ms.count());
                    }
            };
        }
    }
}
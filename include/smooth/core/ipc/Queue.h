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
            /// T Queue<T> is precively what that name suggest - a queue that holds items of type T.
            /// It is also thread-safe. It can be used either as a stand alone queue or as the base for
            /// more specialized implementations, such as the TaskEventQueue and SubscribingTaskEventQueue.
            /// Please note that this implementation supports actual C++ objects as opposed to the FreeRTOS
            /// plain data-only queues. This means that you can place any type of C++ object on these queues
            /// as long as the objects are copyable (the default copy constructor and assignment operator are enough)
            /// Items are placed on the queue by copy, not by reference.
            /// \tparam T The type of object to hold in the queue.
            template<typename T>
            class Queue
            {
                public:
                    /// Constructor
                    /// \param name The name of the queue, mainly used for debugging and logging.
                    /// \param size The size of the queue, i.e. the number of items it can hold.
                    /// The total number of bytes allocated is size * sizeof(T) + size * sizeof(uint8_t).
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

                    /// Destructor
                    virtual ~Queue()
                    {
                        smooth::core::ipc::Mutex::Lock lock(guard);
                        if (handle != nullptr)
                        {
                            vQueueDelete(handle);
                        }
                        items.clear();
                    }

                    /// Gets the size of the queue.
                    /// \return number of items the queue can hold.
                    int size()
                    {
                        smooth::core::ipc::Mutex::Lock lock(guard);
                        return queue_size;
                    }

                    /// Gets the underlying FreeRTOS handle for the FreeRTOS queue.
                    /// Unless you are implementing a specialization of the Queue, you
                    /// shouldn't need to call this method.
                    /// \return The queue handle
                    QueueHandle_t get_handle() const
                    {
                        return handle;
                    }

                    /// Pushes an item into the queue
                    /// \param item The item of which a copy will be placed on the queue.
                    /// \return true if the queue could accept the item, otherwise false.
                    bool push(const T& item)
                    {
                        return push(item, std::chrono::milliseconds(0));
                    }

                    /// /// Pushes an item into the queue
                    /// \param item The item of which a copy will be placed on the queue.
                    /// \param wait_time The time to wait for a slot to become available in the queue before failing.
                    /// \return true if the queue could accept the item, otherwise false.
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

                    /// Pops an item off the queue.
                    /// \param target A reference to an instance of T which will be assigned the item taken from the queue.
                    /// \return true if an item could be received, otherwise false.
                    bool pop(T& target)
                    {
                        return pop(target, std::chrono::milliseconds(0));
                    }

                    /// Pops an item off the queue.
                    /// \param target A reference to an instance of T which will be assigned the item taken from the queue.
                    /// \param wait_time The time to wait for an item to become available on the queue.
                    /// \return true if an item could be received, otherwise false.
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

                    /// Returns a value indicating if the queue is empty.
                    /// \return true if empty, otherwise false.
                    bool empty()
                    {
                        return count() == 0;
                    }

                    /// Returns the number of items waiting to be popped.
                    /// \return The number of items in the queue.
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
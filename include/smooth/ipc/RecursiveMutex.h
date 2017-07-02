//
// Created by permal on 6/26/17.
//

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <chrono>

namespace smooth
{
    namespace ipc
    {
        class RecursiveMutex
        {
            public:

                // Class used to do scoped-based locking.
                class Lock
                {
                    public:
                        Lock(RecursiveMutex& m)
                                : m(m)
                        {
                            m.acquire();
                        }

                        ~Lock()
                        {
                            m.release();
                        }

                    private:
                        RecursiveMutex& m;
                };


                RecursiveMutex() = default;

                RecursiveMutex(const Mutex&) = delete;

                // Blocks until the mutex is acquired
                void acquire()
                {
                    xSemaphoreTakeRecursive(semaphore, portMAX_DELAY);
                }

                bool acquire(std::chrono::milliseconds wait_time)
                {
                    return xSemaphoreTakeRecursive(semaphore, wait_time.count() / portTICK_PERIOD_MS) == pdTRUE;
                }

                void release()
                {
                    xSemaphoreGiveRecursive(semaphore);
                }

            private:
                SemaphoreHandle_t semaphore = xSemaphoreCreateRecursiveMutex();
        };

    }
}

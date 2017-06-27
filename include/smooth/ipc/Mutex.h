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
        class Mutex
        {
            public:

                // Class used to do scoped-based locking.
                class Lock
                {
                    public:
                        Lock(Mutex& m)
                                : m(m)
                        {
                            m.acquire();
                        }

                        ~Lock()
                        {
                            m.release();
                        }

                    private:
                        Mutex& m;
                };


                Mutex() = default;

                Mutex(const Mutex&) = delete;

                // Blocks until the mutex is acquired
                void acquire()
                {
#if INCLUDE_vTaskSuspend != 1
    #error "INCLUDE_vTaskSuspend must be configured to 1 to allow blocking indefinately"
#endif
                    xSemaphoreTake(semaphore, portMAX_DELAY);
                }

                bool acquire(std::chrono::milliseconds wait_time)
                {
                    return xSemaphoreTake(semaphore, wait_time.count() / portTICK_PERIOD_MS) == pdTRUE;
                }

                bool acquire_from_isr()
                {
                    return xSemaphoreTakeFromISR(semaphore, nullptr) == pdTRUE;
                }

                void release()
                {
                    xSemaphoreGive(semaphore);
                }

                void release_from_isr()
                {
                    xSemaphoreGiveFromISR(semaphore, nullptr);
                }

            private:
                SemaphoreHandle_t semaphore = xSemaphoreCreateMutex();
        };

    }
}

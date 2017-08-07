//
// Created by permal on 6/26/17.
//

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <chrono>

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            /// A mutex for task synchronization
            class Mutex
            {
                public:

                    /// Class used to do scoped-based locking of a Mutex.
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

                    /// Acquires the mutex, blocking until the mutex is acquired.
                    void acquire()
                    {
#if INCLUDE_vTaskSuspend != 1
#error "INCLUDE_vTaskSuspend must be configured to 1 to allow blocking indefinately"
#endif
                        xSemaphoreTake(semaphore, portMAX_DELAY);
                    }

                    /// Acquires the mutex, blocking at most the specified time.
                    /// \return true if the mutex was acquired, otherwise false.
                    bool acquire(std::chrono::milliseconds wait_time)
                    {
                        return xSemaphoreTake(semaphore, wait_time.count() / portTICK_PERIOD_MS) == pdTRUE;
                    }

                    /// Acquires the mutex from an ISR, blocking until the mutex is acquired.
                    bool acquire_from_isr()
                    {
                        return xSemaphoreTakeFromISR(semaphore, nullptr) == pdTRUE;
                    }

                    /// Releases the mutex.
                    void release()
                    {
                        xSemaphoreGive(semaphore);
                    }

                    /// Releases the from an ISR.
                    void release_from_isr()
                    {
                        xSemaphoreGiveFromISR(semaphore, nullptr);
                    }

                private:
                    SemaphoreHandle_t semaphore = xSemaphoreCreateMutex();
            };

        }
    }
}
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
            /// A mutex for task synchronization allowing for recursive locks from the same task.
            class RecursiveMutex
            {
                public:

                    /// Class used to do scoped-based locking of an RecursiveMutex.
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

                    RecursiveMutex(const RecursiveMutex&) = delete;

                    /// Acquires the mutex, blocking until the mutex is acquired.
                    void acquire()
                    {
#if INCLUDE_vTaskSuspend != 1
#error "INCLUDE_vTaskSuspend must be configured to 1 to allow blocking indefinately"
#endif
                        xSemaphoreTakeRecursive(semaphore, portMAX_DELAY);
                    }

                    /// Acquires the mutex, blocking at most the specified time.
                    /// \return true if the mutex was acquired, otherwise false.
                    bool acquire(std::chrono::milliseconds wait_time)
                    {
                        return xSemaphoreTakeRecursive(semaphore, wait_time.count() / portTICK_PERIOD_MS) == pdTRUE;
                    }

                    ///Releases the mutex. The number of calls to release() must match with the number of calls
                    /// to acquire for the mutex to be released.
                    void release()
                    {
                        xSemaphoreGiveRecursive(semaphore);
                    }

                private:
                    SemaphoreHandle_t semaphore = xSemaphoreCreateRecursiveMutex();
            };

        }
    }
}
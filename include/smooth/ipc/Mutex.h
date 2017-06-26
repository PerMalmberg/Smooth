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
                Mutex()
                {
                }

                Mutex(const Mutex&) = delete;

                // Default to 1 tick wait_time
                bool acquire(std::chrono::milliseconds wait_time = std::chrono::milliseconds(portTICK_PERIOD_MS))
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

//
// Created by permal on 6/25/17.
//

#pragma once

#include <string>
#include <chrono>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <forward_list>
#include <smooth/ipc/Queue.h>
#include <smooth/ipc/ITaskEventQueue.h>
#include <smooth/ipc/Mutex.h>

namespace smooth
{
    // The Task class encapsulates management and execution of a task.
    class Task
    {
        public:
            virtual ~Task();
            void start();
            // This is static so that it can be used also in app_main().
            static void delay(std::chrono::milliseconds ms)
            {
                vTaskDelay(ms.count() / portTICK_PERIOD_MS);
            }

            static void never_return()
            {
                for (;;)
                {
                    smooth::Task::delay(std::chrono::seconds(1));
                }
            }

            void message_available(ipc::ITaskEventQueue* queue);

        protected:
            Task(const std::string& task_name, uint32_t stack_depth, UBaseType_t priority, int max_waiting_messages, std::chrono::milliseconds tick_interval);

            // The tick() method is where the task shall perform its work.
            // It is called every 'tick_interval' when there no events available.
            // Note that if there is a constant stream of event received via a TaskEventQueue,
            // then the tick may be delayed (depending on the tick_interval).
            virtual void tick() {};

        private:
            std::string name;
            TaskHandle_t task_handle = nullptr;
            uint32_t stack_depth;
            UBaseType_t priority;
            std::chrono::milliseconds tick_interval;
            ipc::Queue<ipc::ITaskEventQueue*> notification;

            void exec(void);
    };
}

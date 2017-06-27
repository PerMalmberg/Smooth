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
#include <smooth/ipc/ITaskEventQueue.h>

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

            void message_available();

            void register_event_queue( ipc::ITaskEventQueue* queue );

        protected:
            Task(const std::string& task_name, uint32_t stack_depth, UBaseType_t priority);

            // The loop() method is where the task shall perform its work.
            // As it must never return, the method is called over and over.
            virtual void loop() = 0;

        private:
            std::string name;
            TaskHandle_t task_handle = nullptr;
            uint32_t stack_depth;
            UBaseType_t priority;
            std::forward_list<ipc::ITaskEventQueue*> registered_queues;

            void exec(void);
    };
}

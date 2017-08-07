//
// Created by permal on 6/25/17.
//

#pragma once

#include <string>
#include <chrono>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <map>
#include <smooth/core/ipc/Queue.h>
#include <smooth/core/ipc/ITaskEventQueue.h>
#include <smooth/core/ipc/Mutex.h>

namespace smooth
{
    namespace core
    {
        /// The Task class encapsulates management and execution of a task.
        /// The intent is to provide the scaffolding needed by nearly every task in an
        /// embedded system; an initialization method, a periodically called tick(),
        /// the ability to receive events in a thread-safe manner.
        class Task
        {
            public:
                virtual ~Task();

                /// Starts the task.
                void start();

                // This is static so that it can be used also in app_main().
                static void delay(std::chrono::milliseconds ms)
                {
                    vTaskDelay(ms.count() / portTICK_PERIOD_MS);
                }

                void register_queue_with_task(smooth::core::ipc::ITaskEventQueue* task_queue);

            protected:

                // Use this constructor to attach to an existing task, i.e. the main task.
                Task(TaskHandle_t task_to_attach_to, UBaseType_t priority, std::chrono::milliseconds tick_interval);

                Task(const std::string& task_name, uint32_t stack_depth, UBaseType_t priority,
                     std::chrono::milliseconds tick_interval);

                /// The tick() method is where the task shall perform its work.
                /// It is called every 'tick_interval' when there no events available.
                /// Note that if there is a constant stream of event received via a TaskEventQueue,
                /// the tick may be delayed (depending on the tick_interval).
                virtual void tick()
                {
                };

                /// Called once when task is started.
                virtual void init()
                {
                }

            private:
                std::string name;
                TaskHandle_t task_handle = nullptr;
                uint32_t stack_depth;
                UBaseType_t priority;
                std::chrono::milliseconds tick_interval;
                QueueSetHandle_t notification;
                std::map<QueueSetMemberHandle_t, smooth::core::ipc::ITaskEventQueue*> queues{};
                bool is_attached = false;
                bool started = false;

                void exec();
                void prepare_queues();
        };
    }
}
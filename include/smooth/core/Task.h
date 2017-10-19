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
#include <smooth/core/ipc/Lock.h>
#include <thread>
#include <smooth/core/ipc/QueueNotification.h>

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

                /// Delay for the specified amount
                /// \param ms Time to wait
                static void delay(std::chrono::milliseconds ms)
                {
                    std::this_thread::sleep_for(ms);
                }

                /// Convert time to ticks
                /// \param ms Time
                /// \return Ticks
                static inline TickType_t to_tick(std::chrono::milliseconds ms)
                {
                    return pdMS_TO_TICKS(ms.count());
                }

                void register_queue_with_task(smooth::core::ipc::ITaskEventQueue* task_queue);

            protected:

                /// Use this constructor to attach to an existing task, i.e. the main task.
                /// \param task_to_attach_to The task to attach to.
                /// \param priority Task priority
                /// \param tick_interval Tick interval
                Task(TaskHandle_t task_to_attach_to, UBaseType_t priority, std::chrono::milliseconds tick_interval);

                /// Use this constructor when creating your own task.
                /// \param task_name Name of task.
                /// \param stack_size Tack size, in bytes.
                /// \param priority Task priority
                /// \param tick_interval Tick interval
                Task(const std::string& task_name, uint32_t stack_size, UBaseType_t priority,
                     std::chrono::milliseconds tick_interval);

                /// The tick() method is where the task shall perform its work.
                /// It is called every 'tick_interval' when there no events available.
                /// Note that if there is a constant stream of event received via a TaskEventQueue,
                /// the tick may be delayed (depending on the tick_interval).
                virtual void tick()
                {
                }

                /// Called once when task is started.
                virtual void init()
                {
                }

                /// Gets the tick interval
                /// \return The tick interval.
                std::chrono::milliseconds get_tick_interval() const
                {
                    return tick_interval;
                }

                // Prints the task information.
                void print_task_info();

            private:
                std::string name;
                TaskHandle_t task_handle = nullptr;
                uint32_t stack_size;
                uint32_t priority;
                std::chrono::milliseconds tick_interval;
                smooth::core::ipc::QueueNotification notification;

                bool is_attached = false;
                bool started = false;

                void exec();
                void prepare_queues();
        };
    }
}
//
// Created by permal on 6/25/17.
//

#include <smooth/core/Task.h>
#include <smooth/core/timer/ElapsedTime.h>
#include <algorithm>

namespace smooth
{
    namespace core
    {
        Task::Task(const std::string& task_name, uint32_t stack_size, uint32_t priority,
                   std::chrono::milliseconds tick_interval)
                : name(task_name),
                  stack_size(stack_size),
                  priority(priority),
                  tick_interval(tick_interval),
                  notification()
        {
        }

        Task::Task(TaskHandle_t task_to_attach_to, uint32_t priority, std::chrono::milliseconds tick_interval)
                :
                name("MainTask"),
                task_handle(task_to_attach_to),
                stack_size(0),
                priority(priority),
                tick_interval(tick_interval),
                notification()
        {
            vTaskPrioritySet(task_handle, priority);
            is_attached = true;
        }

        Task::~Task()
        {
            notification.clear();
            vTaskDelete(task_handle);
        }

        void Task::start()
        {
            // Prevent multiple starts
            if (!started)
            {
                started = true;

                if (is_attached)
                {
                    // Attaching to another task, just run execute.
                    exec();
                }
                else
                {
                    xTaskCreate(
                            [](void* o)
                            {
                                static_cast<Task*>(o)->exec();
                            },
                            name.c_str(), stack_size, this, priority, &task_handle);
                }
            }
        }

        void Task::exec()
        {
            ESP_LOGV("Task", "Initializing task '%s', %p", pcTaskGetTaskName(task_handle), task_handle);

            init();

            ESP_LOGV("Task", "Task '%s' initialized, %p", pcTaskGetTaskName(task_handle), task_handle);

            timer::ElapsedTime delayed{};

            delayed.start();

            for (;;)
            {

                // Try to keep the tick alive even when there are lots of incoming messages
                // by simply not checking the queues when more than one tick interval has passed.
                if (tick_interval.count() > 0 && delayed.get_running_time() > tick_interval)
                {
                    tick();
                    delayed.reset();
                }
                else
                {
                    auto* queue = notification.wait_for_notification(tick_interval);

                    if (queue == nullptr)
                    {
                        // Timeout - no messages.
                        tick();
                        delayed.reset();
                    }
                    else
                    {
                        // A queue has signaled an item is available.
                        // Note: do not get tempted to retrieve all messages from
                        // the queue - it would cause message ordering to get mixed up.
                        queue->forward_to_event_queue();
                    }
                }
            }
        }


        void Task::register_queue_with_task(smooth::core::ipc::ITaskEventQueue* task_queue)
        {
            task_queue->register_notification(&notification);
        }

        void Task::print_task_info()
        {
            ESP_LOGI("TaskInfo", "%s: Stack: %u, Min heap: %u", name.c_str(), uxTaskGetStackHighWaterMark(nullptr), esp_get_minimum_free_heap_size());
        }
    }
}
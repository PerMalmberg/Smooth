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
        Task::Task(const std::string& task_name, uint32_t stack_size, UBaseType_t priority,
                   std::chrono::milliseconds tick_interval)
                : name(task_name),
                  stack_size(stack_size),
                  priority(priority),
                  tick_interval(tick_interval),
                  notification(nullptr)
        {
        }

        Task::Task(TaskHandle_t task_to_attach_to, UBaseType_t priority, std::chrono::milliseconds tick_interval)
                :
                name("MainTask"),
                task_handle(task_to_attach_to),
                stack_size(0),
                priority(priority),
                tick_interval(tick_interval),
                notification(nullptr)
        {
            vTaskPrioritySet(task_handle, priority);
            is_attached = true;
        }

        Task::~Task()
        {
            for (auto& q : queues)
            {
                xQueueRemoveFromSet(q.second->get_handle(), notification);
            }

            queues.clear();
            vTaskDelete(task_handle);
        }

        void Task::start()
        {
            // Prevent multiple starts
            if (!started)
            {
                started = true;

                prepare_queues();

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
                    // Limit task tick to a minimum of 1 tick.
                    QueueSetMemberHandle_t queue = xQueueSelectFromSet(notification,
                                                                       std::max(static_cast<TickType_t>(1),
                                                                                pdMS_TO_TICKS(
                                                                                        tick_interval.count())));

                    if (queue == nullptr)
                    {
                        // Timeout - no messages.
                        tick();
                        delayed.reset();
                    }
                    else
                    {
                        // A queue or mutex has signaled an item is available.
                        auto it = queues.find(queue);
                        if (it != queues.end())
                        {
                            it->second->forward_to_event_queue();
                        }
                    }
                }
            }
        }

        void Task::prepare_queues()
        {
            int queue_set_size = 0;
            // Add all queues belonging to this Task.
            for (auto& q : queues)
            {
                queue_set_size += q.second->size();
            }

            // Create the queue notification set, always 1 slots or greater to to handle
            // tasks without any queues.
            notification = xQueueCreateSet(std::max(1, queue_set_size));

            for (auto& q : queues)
            {
                xQueueAddToSet(q.second->get_handle(), notification);
            }
        }

        void Task::register_queue_with_task(smooth::core::ipc::ITaskEventQueue* task_queue)
        {
            // The notification queue must be be able to hold the total number of possible waiting messages.
            queues.insert(std::make_pair(task_queue->get_handle(), task_queue));
        }

        void Task::print_task_info()
        {
            ESP_LOGI( "TaskInfo", "%s: Stack: %u", name.c_str(), uxTaskGetStackHighWaterMark(nullptr));
        }
    }
}
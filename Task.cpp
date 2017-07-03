//
// Created by permal on 6/25/17.
//

#include "smooth/Task.h"

namespace smooth
{

    Task::Task(const std::string& task_name, uint32_t stack_depth, UBaseType_t priority,
               std::chrono::milliseconds tick_interval)
            : name(task_name),
              stack_depth(stack_depth),
              priority(priority),
              tick_interval(tick_interval),
              notification( std::string("Notification-") + name, 1)
    {
    }

    Task::~Task()
    {
        vTaskDelete(task_handle);
    }

    void Task::start()
    {
        // Prevent multiple starts
        if (task_handle == nullptr)
        {
            // Resize the notification queue
            notification.set_size(notification_size);

            xTaskCreate(
                    [](void* o)
                    {
                        static_cast<Task*>(o)->exec();
                    },
                    name.c_str(), stack_depth, this, priority, &task_handle);
        }
    }

    void Task::exec(void)
    {
        for (;;)
        {
            ipc::ITaskEventQueue* queue;
            if (notification.pop(queue, tick_interval))
            {
                // An event has arrived, get the queue to forward it to us.
                queue->forward_to_task();
            }
            else
            {
                // Timeout, perform tick.
                tick();
            }
        }
    }

    void Task::message_available(ipc::ITaskEventQueue* queue)
    {
        // Enqueue the queue that has a message available
        notification.push(queue);
    }

    void Task::report_queue_size(int size)
    {
        // The notification queue must be be able to hold the total number of possible waiting messages.
        notification_size += size;
    }


}
//
// Created by permal on 6/25/17.
//

#include "smooth/Task.h"

namespace smooth
{

    Task::Task(const std::string& task_name, uint32_t stack_depth, UBaseType_t priority)
            : name(task_name),
              stack_depth(stack_depth),
              priority(priority),
              registered_queues()
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
            loop();
            taskYIELD();
        }
    }

    void Task::message_available()
    {
        // QQQ Implement a common wait with timeout for all queues.
        // On timeout, call tick(), i..e. currently loop();
        for( auto* queue : registered_queues )
        {
            queue->pop_next();
        }
    }

    void Task::register_event_queue(ipc::ITaskEventQueue* queue)
    {
        registered_queues.push_front(queue);
    }

}
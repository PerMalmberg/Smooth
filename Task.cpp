//
// Created by permal on 6/25/17.
//

#include "IDFApp/Task.h"

namespace idfapp
{

    Task::Task(const std::string& task_name, uint32_t stack_depth, UBaseType_t priority)
            : name(task_name),
              stack_depth(stack_depth),
              priority(priority)
    {
    }

    Task::~Task()
    {
        vTaskDelete(task_handle);
    }

    void Task::start()
    {
        // Prevent multiple starts
        if( task_handle == nullptr)
        {
            xTaskCreate(
                    [](void* o)
                    {
                        static_cast<Task*>(o)->exec();
                    },
                    name.c_str(), stack_depth, this, priority, &task_handle);
        }
    }

}
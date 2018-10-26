#pragma once

#include <string>
#include <memory>

#ifdef ESP_PLATFORM
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif

namespace smooth
{
    namespace core
    {
        class TaskStatus
        {
            public:
                TaskStatus()
                        : task_name(),
                          stack_size(0),
                          remaining_stack(0)
                {
                }

                TaskStatus(const TaskStatus& other) = default;
                TaskStatus& operator=(const TaskStatus& other) = default;

#ifdef ESP_PLATFORM
                TaskStatus(std::string task_name, uint32_t stack_size)
                        : task_name(std::move(task_name)),
                          stack_size(stack_size),
                          remaining_stack(uxTaskGetStackHighWaterMark(nullptr))
                {
                }
#else

                TaskStatus(std::string task_name, uint32_t stack_size)
                        : task_name(std::move(task_name)),
                          stack_size(stack_size),
                          remaining_stack(0)
                {
                }

#endif

                const std::string& get_name() const
                {
                    return task_name;
                }

                uint32_t get_stack_size() const
                {
                    return stack_size;
                }

                uint32_t get_remaining_stack() const
                {
                    return remaining_stack;
                }

            private:
                std::string task_name;
                uint32_t stack_size;
                uint32_t remaining_stack;

        };
    }
}

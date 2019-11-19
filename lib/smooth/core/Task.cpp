/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <utility>
#include <algorithm>
#include "smooth/core/Task.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/ipc/Publisher.h"
#include "smooth/core/SystemStatistics.h"

#ifdef ESP_PLATFORM
#include <esp_pthread.h>
#include <freertos/task.h>
#endif

using namespace smooth::core::logging;

namespace smooth::core
{
    /// Constructor used when creating a new task running on a new thread.
    Task::Task(std::string task_name, uint32_t stack_size, uint32_t priority,
               std::chrono::milliseconds tick_interval, int core)
            : name(std::move(task_name)),
              worker(),
              stack_size(stack_size),
              priority(priority),
              tick_interval(tick_interval),
              is_attached(false),
              affinity(core)
    {
    }

    /// Constructor used when attaching to an already running thread.
    Task::Task(uint32_t priority, std::chrono::milliseconds tick_interval)
            : name("MainTask"),
              stack_size(0),
              priority(priority),
              tick_interval(tick_interval),
              is_attached(true),
              affinity(tskNO_AFFINITY)
    {
#ifdef ESP_PLATFORM
        stack_size = CONFIG_MAIN_TASK_STACK_SIZE;
#endif
    }

    Task::~Task()
    {
        notification.clear();
    }

    void Task::start()
    {
        std::unique_lock<std::mutex> lock{ start_mutex };

        // Prevent multiple starts
        if (!started)
        {
            status_report_timer.start();

            if (is_attached)
            {
                Log::debug(name, "Running as attached thread");

                // Attaching to another task, just run execute.
                exec();
            }
            else
            {
#ifdef ESP_PLATFORM

                // Since std::thread is implemented using pthread, setting the config before
                // creating the std::thread we get the desired effect, even if we're not calling
                // pthread_create() as per the IDF documentation.
                auto worker_config = esp_pthread_get_default_config();
                worker_config.stack_size = stack_size;
                worker_config.prio = priority;
                worker_config.thread_name = name.c_str();

                // Set to desired core, otherwise use default (as per config).
                if (affinity != tskNO_AFFINITY)
                {
                    worker_config.pin_to_core = affinity;
                }

                esp_pthread_set_cfg(&worker_config);
#endif
                Log::debug(name, "Creating worker thread");
                worker = std::thread([this]() {
                                         this->exec();
                });

                Log::debug(name, "Waiting for worker to start");

                // To avoid race conditions between tasks during start up,
                // always wait for the new task to start.
                start_condition.wait(lock,
                                     [this] {
                                         return started.load();
                                     });

                Log::debug(name, "Worker started");
            }
        }
    }

    void Task::exec()
    {
        Log::debug(name, "Executing...");

        if (!is_attached)
        {
            Log::debug(name, "Notify start_mutex");
            started = true;
            std::unique_lock<std::mutex> lock{ start_mutex };
            start_condition.notify_all();
        }

        Log::verbose(name, "Initializing...");
        init();

        Log::verbose(name, "Initialized");

        timer::ElapsedTime delayed{};

        delayed.start();

        report_stack_status();

        for (;; )
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
                std::unique_lock<std::mutex> lock{ queue_mutex };

                // Check the polled queues for data availability
                for (auto q : polled_queues)
                {
                    q->poll();
                }

                // Wait for data to become available, or a timeout to occur.
                auto queue_ptr = notification.wait_for_notification(tick_interval);

                // Check if the weak_ptr we got back is uninitialized using the defined behaviour here:
                // https://en.cppreference.com/w/cpp/memory/weak_ptr/owner_before
                //
                // Quote: The order is such that two smart pointers compare equivalent only
                // if they are both empty or if they both own the same object, even if the
                // values of the pointers obtained by get() are different (e.g. because they
                // point at different subobjects within the same object)
                decltype(queue_ptr) empty_ptr{};

                if (!queue_ptr.owner_before(empty_ptr) && !empty_ptr.owner_before(queue_ptr))
                {
                    // Timeout - no messages.
                    tick();
                    delayed.reset();
                }
                else
                {
                    // A queue has signaled an item is available.
                    // Note: Do not retrieve all messages from the the queue;
                    // it will prevent messages to arrive in the same order
                    // they were sent when there are more than one receiver queue.
                    auto queue = queue_ptr.lock();

                    if (queue)
                    {
                        queue->forward_to_event_listener();
                    }
                }
            }

            if (status_report_timer.get_running_time() > std::chrono::seconds(60))
            {
                report_stack_status();
                status_report_timer.reset();
            }
        }
    }

    void Task::register_queue_with_task(smooth::core::ipc::ITaskEventQueue* task_queue)
    {
        task_queue->register_notification(&notification);
    }

    void Task::register_polled_queue_with_task(smooth::core::ipc::IPolledTaskQueue* polled_queue)
    {
        std::unique_lock<std::mutex> lock{ queue_mutex };
        polled_queue->register_notification(&notification);
        polled_queues.push_back(polled_queue);
    }

    void Task::unregister_polled_queue_with_task(smooth::core::ipc::IPolledTaskQueue* polled_queue)
    {
        std::unique_lock<std::mutex> lock{ queue_mutex };
        auto pos = std::find(polled_queues.begin(), polled_queues.end(), polled_queue);

        if (pos != polled_queues.end())
        {
            polled_queues.erase(pos);
        }
    }

    void Task::report_stack_status()
    {
        SystemStatistics::instance().report(name, TaskStats{ stack_size });
    }
}

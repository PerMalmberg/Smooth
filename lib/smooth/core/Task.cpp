//
// Created by permal on 6/25/17.
//

#include <algorithm>
#include <smooth/core/Task.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/ipc/Publisher.h>

#ifdef ESP_PLATFORM
#include "esp_pthread.h"
#include <freertos/task.h>
#endif

using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        /// Constructor used when creating a new task running on a new thread.
        Task::Task(const std::string& task_name, uint32_t stack_size, uint32_t priority,
                   std::chrono::milliseconds tick_interval, int core)
                : name(task_name),
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
                :
                name("MainTask"),
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
            std::unique_lock<std::mutex> lock{start_mutex};

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
                    if(affinity != tskNO_AFFINITY)
                    {                        
                        worker_config.pin_to_core = affinity;
                    }

                    esp_pthread_set_cfg(&worker_config);
#endif
                    Log::debug(name, "Creating worker thread");
                    worker = std::thread([this]()
                                         {
                                             this->exec();
                                         });

                    Log::debug(name, "Waiting for worker to start");

                    // To avoid race conditions between tasks during start up,
                    // always wait for the new task to start.
                    start_condition.wait(lock,
                                         [this]
                                         {
                                             return started.load();
                                         });

                    Log::debug(name, "Worker started");
                }
            }
        }

        void Task::exec()
        {
            Log::debug(name, "Executing...");

            if(!is_attached)
            {
                Log::debug(name, "Notify start_mutex");
                started = true;
                std::unique_lock<std::mutex> lock{start_mutex};
                start_condition.notify_all();
            }

            Log::verbose(name, "Initializing...");

            init();

            Log::verbose(name, "Initialized");

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
                    std::unique_lock<std::mutex> lock{queue_mutex};
                    // Check the polled queues for data availability
                    for(auto q : polled_queues)
                    {
                        q->poll();
                    }

                    // Wait for data to become available, or a timeout to occur.
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
                        // Note: Do not retrieve all messages from the the queue;
                        // it will prevent messages to arrive in the same order
                        // they were sent to, when there are more than one
                        // receiver queue.
                        queue->forward_to_event_queue();
                    }
                }

                if (status_report_timer.get_running_time() > std::chrono::seconds(60))
                {
                    print_stack_status();
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
            std::unique_lock<std::mutex> lock{queue_mutex};
            polled_queue->register_notification(&notification);
            polled_queues.push_back(polled_queue);
        }

        void Task::unregister_polled_queue_with_task(smooth::core::ipc::IPolledTaskQueue* polled_queue)
        {
            std::unique_lock<std::mutex> lock{queue_mutex};
            auto pos = std::find(polled_queues.begin(), polled_queues.end(), polled_queue);
            if(pos != polled_queues.end())
            {
                polled_queues.erase(pos);
            }
        }

        void Task::print_stack_status()
        {
#ifdef ESP_PLATFORM
            if(status_print_enabled)
            {
                // https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/freertos.html?highlight=uxTaskGetStackHighWaterMark
                auto minimum_free_stack_in_bytes = uxTaskGetStackHighWaterMark(nullptr);
                Format msg("Minimum free stack: {1} of {2} => {3} used.",
                           UInt32(minimum_free_stack_in_bytes),
                           UInt32(stack_size),
                           UInt32(stack_size - minimum_free_stack_in_bytes));

                if (minimum_free_stack_in_bytes <= 256)
                {
                    Log::warning(name, msg);
                }
                else
                {
                    Log::info(name, msg);
                }
            }
#endif
        }
    }
}
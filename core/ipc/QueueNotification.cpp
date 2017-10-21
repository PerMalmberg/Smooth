//
// Created by permal on 10/18/17.
//

#include <thread>
#include <smooth/core/ipc/QueueNotification.h>
#include <smooth/core/timer/ElapsedTime.h>
#include <smooth/core/logging/log.h>


namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            QueueNotification::QueueNotification()
                    : queues(), guard(), cond()
            {
            }

            void QueueNotification::notify(ITaskEventQueue* queue)
            {
                // It might look like the queue can grow without bounds, but that is not the case
                // as TaskEventQueues only call this method when they have successfully added the
                // data item to their internal queue. As such, the queue can only be as large as
                // the sum of all tasks within the same Task.
                std::lock_guard<std::mutex> lock(guard);
                queues.push(queue);
                has_data = true;
#ifdef ESP_PLATFORM
                std::this_thread::yield();
#else
                // TODO: QQQ This is a temporary workaround due to missing functional std::condition_variable in the xtensa-gcc port.
                cond.notify_one();
#endif
            }


            ITaskEventQueue* QueueNotification::wait_for_notification(std::chrono::milliseconds timeout)
            {
                ITaskEventQueue* res = nullptr;

                std::unique_lock<std::mutex> lock(guard);

                if (queues.empty())
                {
#ifdef ESP_PLATFORM
                    // TODO: QQQ This is a temporary workaround due to missing functional std::condition_variable in the xtensa-gcc port.
                    timer::ElapsedTime e;
                    e.start();

                    lock.unlock();
                    while (res == nullptr && e.get_running_time() < timeout)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        lock.lock();
                        if(!queues.empty())
                        {
                            res = queues.front();
                            queues.pop();
                        }
                        lock.unlock();
                    }
#else
                    // Wait until data is available, or timeout. This will atomically release the lock.
                    auto wait_result = cond.wait_until(lock,
                                                       std::chrono::system_clock::now() + timeout,
                                                       [this]()
                                                       {
                                                           // Stop waiting when there is data
                                                           return has_data;
                                                       });

                    // At this point we will have the lock again.
                    if (wait_result)
                    {
                        has_data = false;
                        if (!queues.empty())
                        {
                            res = queues.front();
                            queues.pop();
                        }
                    }
#endif // END ESP_PLATFORM
                }
                else
                {
                    res = queues.front();
                    queues.pop();
                }

                return res;
            }
        }
    }
}

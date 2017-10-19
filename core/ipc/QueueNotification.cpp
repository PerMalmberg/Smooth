//
// Created by permal on 10/18/17.
//

#include <thread>
#include <smooth/core/ipc/QueueNotification.h>
#include "esp_log.h"

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            QueueNotification::QueueNotification()
                    : queues(), guard()
            {
            }

            void QueueNotification::notify(ITaskEventQueue* queue)
            {
                // It might look like the queue can grow without bounds, but that is not the case
                // as TaskEventQueues only call this method when they have successfully added the
                // data item to their internal queue. As such, the queue can onöy be as large as
                // the sum of all tasks within the same Task.
                Lock lock(guard);
                queues.push(queue);
            }


            ITaskEventQueue* QueueNotification::wait_for_notification(std::chrono::milliseconds timeout)
            {
                timer::ElapsedTime e;
                e.start();

                ITaskEventQueue* res = nullptr;

                while (res == nullptr && e.get_running_time() < timeout)
                {
                    std::this_thread::yield();

                    if (guard.try_lock())
                    {
                        if (!queues.empty())
                        {
                            res = queues.front();
                            queues.pop();
                        }
                        guard.unlock();
                    }
                    else
                    {
                        ESP_LOGV("Rec", "%d", __LINE__);
                        std::this_thread::yield();
                    }
                }

                return res;
            }
        }
    }
}

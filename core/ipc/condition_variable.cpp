
#include <smooth/core/ipc/condition_variable.h>
#include <smooth/core/Task.h>
#include <algorithm>

using namespace smooth::core::logging;
using namespace std::chrono;

namespace smooth
{
    namespace core
    {
        namespace ipc
        {
            condition_variable::condition_variable(smooth::core::Task& parent)
                    : parent(parent)
            {
            }

            bool condition_variable::
            wait_for(std::unique_lock<std::mutex>& lock,
                     const std::chrono::steady_clock::duration& duration,
                     std::function<bool()> pred)
            {
                return wait_until(lock, steady_clock::now() + duration, pred);
            }

            bool condition_variable::wait_until(std::unique_lock<std::mutex>& lock,
                                                const std::chrono::steady_clock::time_point& end_time,
                                                std::function<bool()> pred)
            {
                // The task (i.e. our parent) shall now wait for a signal, or a timeout.
                bool timed_out;
#ifdef ESP_PLATFORM
                auto expires_in_ms = duration_cast<milliseconds>(end_time - steady_clock::now());
                uint32_t dummy;

                lock.unlock();

                // Don't let the tick be less than one in order to prevent CPU hogging.
                auto ms = expires_in_ms.count();
                timed_out = xTaskNotifyWait(0, 0, &dummy, std::max(pdMS_TO_TICKS(ms), static_cast<TickType_t>(1))) == pdFALSE;

                if(pred)
                {
                    if(!timed_out)
                    {
                        // Didn't timeout so return whatever the predicate returns.
                        timed_out = pred();
                    }
                }

                lock.lock();
#else
                timed_out = pred ? cond.wait_until(lock, end_time, pred) : cond.wait_until(lock, end_time) == std::cv_status::timeout;
#endif
                return timed_out;
            }

            void condition_variable::notify_one()
            {
#ifdef ESP_PLATFORM
                xTaskNotify(parent.get_freertos_task(), 0, eNoAction);
#else
                cond.notify_one();
#endif
            }
        };
    }
}
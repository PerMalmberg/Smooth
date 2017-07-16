//
// Created by permal on 7/15/17.
//

#include <smooth/timer/Timer.h>
#include "esp_log.h"
#include "esp_attr.h"

namespace smooth
{
    namespace timer
    {
        Timer::Timer(const std::string& name, int id, ipc::TaskEventQueue<TimerExpiredEvent>& event_queue,
                     bool auto_reload, std::chrono::milliseconds interval)
                : name(name), id(id), auto_reload(auto_reload), interval(interval), event_queue(event_queue)
        {
            ESP_LOGD("timer-ctor", "%p", this);

            handle = xTimerCreate(name.c_str(),
                                  pdMS_TO_TICKS(interval.count()),
                                  auto_reload ? 1 : 0,
                                  this, // Use ourselves as the timer id.
                                  [](void* o)
                                  {
                                      // A bit ugly, but required - convert the timer's ID into a
                                      // pointer to the Timer instance.
                                      auto timer_id = pvTimerGetTimerID(o);
                                      Timer* timer = static_cast<Timer*>(timer_id);

                                      if (timer != nullptr)
                                      {
                                          timer->expired();
                                      }
                                  });

            if (handle == nullptr)
            {
                ESP_LOGD("Timer", "Could not create timer, aborting.");
                abort();
            }
        }

        void Timer::start()
        {
            xTimerStart(handle, 0);
        }

        void IRAM_ATTR Timer::start_from_isr()
        {
            BaseType_t higher_priority_task_woken = pdFALSE;
            xTimerStartFromISR(handle, &higher_priority_task_woken);
        }

        void Timer::stop()
        {
            xTimerStop(handle, 0);
        }

        void IRAM_ATTR Timer::stop_from_isr()
        {
            BaseType_t higher_priority_task_woken = pdFALSE;
            xTimerStopFromISR(handle, &higher_priority_task_woken);
        }

        void Timer::reset()
        {
            xTimerReset(handle, 0);
        }

        void IRAM_ATTR Timer::reset_from_isr()
        {
            BaseType_t higher_priority_task_woken = pdFALSE;
            xTimerResetFromISR(handle, &higher_priority_task_woken);
        }

        int Timer::get_id()
        {
            return id;
        }

        const std::string& Timer::get_name()
        {
            return name;
        }

        void Timer::expired()
        {
            TimerExpiredEvent ev(this);
            event_queue.push(ev);
        }
    }
}
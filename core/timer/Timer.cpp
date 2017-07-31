//
// Created by permal on 7/15/17.
//

#include <smooth/core/timer/Timer.h>
#include "esp_log.h"
#include "esp_attr.h"

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            Timer::Timer(const std::string& name, int id, ipc::TaskEventQueue<TimerExpiredEvent>& event_queue,
                         bool auto_reload, std::chrono::milliseconds interval)
                    : name(name), id(id), auto_reload(auto_reload), interval(interval), event_queue(event_queue)
            {
                create();
            }

            Timer::~Timer()
            {
                destroy();
            }

            void Timer::start()
            {
                start(interval);
            }

            void Timer::start(std::chrono::milliseconds interval)
            {
                active = true;

                // Ensure at least one tick interval
                auto tick_count = std::max( pdMS_TO_TICKS(interval.count()), 1u );

                if (interval != this->interval)
                {
                    this->interval = interval;
                    // Changing the period also starts the timer.
                    xTimerChangePeriod(handle, tick_count, 1);
                }
                else if (!xTimerIsTimerActive(handle))
                {
                    reset();
                }
            }

            void IRAM_ATTR Timer::start_from_isr()
            {
                active = true;
                BaseType_t higher_priority_task_woken = pdFALSE;
                xTimerStartFromISR(handle, &higher_priority_task_woken);
            }

            void Timer::stop()
            {
                active = false;
                xTimerStop(handle, 0);
            }

            void IRAM_ATTR Timer::stop_from_isr()
            {
                BaseType_t higher_priority_task_woken = pdFALSE;
                xTimerStopFromISR(handle, &higher_priority_task_woken);
            }

            void Timer::reset()
            {
                active = true;
                xTimerReset(handle, 0);
            }

            void IRAM_ATTR Timer::reset_from_isr()
            {
                BaseType_t higher_priority_task_woken = pdFALSE;
                xTimerResetFromISR(handle, &higher_priority_task_woken);
            }

            int Timer::get_id() const
            {
                return id;
            }

            const std::string& Timer::get_name()
            {
                return name;
            }

            void Timer::create()
            {
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
                                              if( timer->active )
                                              {
                                                  timer->expired();
                                              }
                                          }
                                      });

                if (handle == nullptr)
                {
                    ESP_LOGE("Timer", "Could not create timer '%s', aborting.", name.c_str());
                    abort();
                }
            }

            void Timer::destroy()
            {
                stop();
                xTimerDelete(handle, 0);
            }

            void Timer::expired()
            {
                TimerExpiredEvent ev(this);
                event_queue.push(ev);
            }
        }
    }
}
//
// Created by permal on 7/13/17.
//

#pragma once

#include <string>
#include <chrono>
#include <functional>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <smooth/timer/Timer.h>
#include <smooth/timer/TimerExpiredEvent.h>
#include <smooth/ipc/TaskEventQueue.h>

namespace smooth
{
    namespace timer
    {
        class Timer
                : public ITimer
        {
            public:
                Timer(const std::string& name, int id, ipc::TaskEventQueue<timer::TimerExpiredEvent>& event_queue, bool auto_reload, std::chrono::milliseconds interval);

                void start() override;
                void IRAM_ATTR start_from_isr() override;
                void stop() override;
                void IRAM_ATTR stop_from_isr() override;
                void reset() override;
                void IRAM_ATTR reset_from_isr() override;
                int get_id() override;
                const std::string& get_name() override;

            protected:
                const std::string name;
                int id;
                bool auto_reload;
                std::chrono::milliseconds interval;

            private:
                void expired();
                TimerHandle_t handle;
                ipc::TaskEventQueue<TimerExpiredEvent>& event_queue;
                int data = 0xA5A5;
        };
    }
}

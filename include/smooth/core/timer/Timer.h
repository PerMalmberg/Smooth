//
// Created by permal on 7/13/17.
//

#pragma once

#include <string>
#include <chrono>
#include <functional>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <smooth/core/timer/Timer.h>
#include <smooth/core/timer/TimerExpiredEvent.h>
#include <smooth/core/ipc/TaskEventQueue.h>

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            class Timer
                    : public ITimer
            {
                public:
                    Timer(const std::string& name, int id, ipc::TaskEventQueue<timer::TimerExpiredEvent>& event_queue,
                          bool auto_reload, std::chrono::milliseconds interval);

                    virtual ~Timer();

                    void start() override;
                    void start(std::chrono::milliseconds interval) override;
                    void IRAM_ATTR start_from_isr() override;
                    void stop() override;
                    void IRAM_ATTR stop_from_isr() override;
                    void reset() override;
                    void IRAM_ATTR reset_from_isr() override;
                    int get_id() const override;
                    const std::string& get_name() override;

                protected:
                    const std::string name;
                    int id;
                    bool auto_reload;
                    std::chrono::milliseconds interval;

                private:
                    void create();
                    void destroy();
                    void expired();
                    TimerHandle_t handle;
                    ipc::TaskEventQueue<TimerExpiredEvent>& event_queue;
                    bool active = false;
            };
        }
    }
}
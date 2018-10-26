//
// Created by permal on 2018-10-26.
//

#pragma once

#include <smooth/core/Application.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/timer/Timer.h>
namespace timer
{
    class App
            : public smooth::core::Application,
              public smooth::core::ipc::IEventListener<smooth::core::timer::TimerExpiredEvent>
    {
    public:

        App();

        void init() override;

        uint32_t count = 0;

        void event(const smooth::core::timer::TimerExpiredEvent& event) override;

    private:
        void create_timer(std::chrono::milliseconds interval);


        struct TimerInfo
        {
            std::shared_ptr<smooth::core::timer::Timer> timer;
            std::chrono::milliseconds interval;
            std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
            uint64_t count = 0;
            std::chrono::milliseconds total = std::chrono::milliseconds(0);
        };

        smooth::core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent> queue;
        std::vector<TimerInfo> timers;
    };
}
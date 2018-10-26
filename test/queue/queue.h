//
// Created by permal on 2018-10-26.
//

#pragma once
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/timer/ElapsedTime.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/Application.h>

namespace queue
{
    class SenderTask
            : public smooth::core::Task
    {
    public:
        explicit SenderTask(smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime>& out);

        void tick() override;

    private:
        smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime>& out;
    };

    class App
            : public smooth::core::Application,
              public smooth::core::ipc::IEventListener<smooth::core::timer::ElapsedTime>
    {
    public:

        App();

        void init() override;

        uint32_t count = 0;
        std::chrono::microseconds total = std::chrono::microseconds(0);

        void event(const smooth::core::timer::ElapsedTime& event) override;

    private:
        smooth::core::ipc::TaskEventQueue<smooth::core::timer::ElapsedTime> queue;
        SenderTask sender;
    };
}
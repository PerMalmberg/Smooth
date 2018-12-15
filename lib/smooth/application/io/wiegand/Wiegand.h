#pragma once

#include <bitset>
#include <smooth/core/Task.h>
#include <smooth/core/io/InterruptInput.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/ISRTaskEventQueue.h>
#include <smooth/core/ipc/ITaskEventQueue.h>
#include <smooth/core/timer/Timer.h>
#include <memory>
#include "IWiegandSignal.h"

class Wiegand : public smooth::core::ipc::IEventListener<smooth::core::io::InterruptInputEvent>,
                public smooth::core::ipc::IEventListener<smooth::core::timer::TimerExpiredEvent>
{
    public:
        Wiegand(smooth::core::Task& task, IWiegandSignal& receiver);
        void event(const smooth::core::io::InterruptInputEvent& event) override;
        void event(const smooth::core::timer::TimerExpiredEvent& event) override;

    private:
        IWiegandSignal& receiver;
        smooth::core::ipc::ISRTaskEventQueue<smooth::core::io::InterruptInputEvent, 40> bit_queue;
        smooth::core::io::InterruptInput d0;
        smooth::core::io::InterruptInput d1;
        std::bitset<34> data{};
        uint8_t bit_count = 0;
        smooth::core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent> line_silent;
        std::shared_ptr<smooth::core::timer::Timer> expire;
};

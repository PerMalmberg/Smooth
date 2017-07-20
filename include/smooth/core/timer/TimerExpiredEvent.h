//
// Created by permal on 7/16/17.
//

#pragma once

#include <smooth/core/timer/ITimer.h>

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            class TimerExpiredEvent
            {
                public:
                    TimerExpiredEvent() = default;

                    TimerExpiredEvent(smooth::core::timer::ITimer* timer)
                            : timer(timer)
                    {
                    }

                    const smooth::core::timer::ITimer* get_timer() const
                    {
                        return timer;
                    }

                private:
                    smooth::core::timer::ITimer* timer = nullptr;
            };
        }
    }
}
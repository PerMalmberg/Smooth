//
// Created by permal on 7/16/17.
//

#pragma once

#include <smooth/timer/ITimer.h>

namespace smooth
{
    namespace timer
    {
        class TimerExpiredEvent
        {
            public:
                TimerExpiredEvent() = default;

                TimerExpiredEvent(smooth::timer::ITimer* timer)
                        : timer(timer)
                {
                }

                const smooth::timer::ITimer* get_timer() const
                {
                    return timer;
                }

            private:
                smooth::timer::ITimer* timer = nullptr;
        };
    }
}

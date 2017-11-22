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
            /// Event sent when a timer expires
            class TimerExpiredEvent
            {
                public:
                    TimerExpiredEvent() = default;

                    explicit TimerExpiredEvent(int id)
                            : id(id)
                    {
                    }

                    /// Gets the timer that has expired
                    /// \return The timer that expired.
                    int get_id() const
                    {
                        return id;
                    }

                private:
                    int id = -1;
            };
        }
    }
}
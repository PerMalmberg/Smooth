//
// Created by permal on 8/1/17.
//

#pragma once

#include <time.h>
#include <sys/time.h>
#include <chrono>

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            /// Performance/time keeping timer. Used to measure the time between to points in time.
            class ElapsedTime
            {
                public:
                    /// Starts and the performance timer.
                    void start()
                    {
                        active = true;
                        zero();
                    }

                    /// Stops the performance timer
                    void stop()
                    {
                        gettimeofday(&end_time, nullptr);
                        active = false;
                        timersub(&end_time, &start_time, &elapsed);
                    }

                    /// Semantically the same as start(), but provided for syntactical reasons.
                    void reset()
                    {
                        // Simply restart the timer.
                        start();
                    }

                    void zero()
                    {
                        gettimeofday(&start_time, nullptr);
                        end_time = start_time;
                    }

                    /// Gets the amount of time passed since start.
                    /// \return The amount of time.
                    std::chrono::microseconds get_running_time();
                    std::chrono::microseconds get_running_time() const;

                private:
                    bool active = false;
                    timeval start_time;
                    // Keep end_time as a member to get slightly more accurate values
                    // since it doesn't need to be constructed on the stack.
                    timeval end_time;
                    timeval elapsed;
            };
        }
    }
}

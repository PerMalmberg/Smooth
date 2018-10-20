//
// Created by permal on 8/1/17.
//

#pragma once

#include <ctime>
#include <sys/time.h>
#include <chrono>

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            /// Performance/time keeping timer. Used to measure the time between two points in time.
            class ElapsedTime
            {
                public:
                    /// Starts the performance timer.
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

                    /// Functionally the same as start(), but provided for syntactical reasons.
                    void reset()
                    {
                        // Simply restart the timer.
                        start();
                    }

                    /// Zeroes the time, but lets it keep running.
                    void zero()
                    {
                        gettimeofday(&start_time, nullptr);
                        end_time = start_time;
                    }

                    /// Stops the timer and zeroes it.
                    void stop_and_zero()
                    {
                        stop();
                        zero();
                    }

                    /// Gets the amount of time passed since start.
                    /// \return The amount of time.
                    std::chrono::microseconds get_running_time();
                    std::chrono::microseconds get_running_time() const;

                    bool is_running() const
                    {
                        return active;
                    }
                private:
                    bool active = false;
                    timeval start_time{};
                    // Keep end_time as a member to get slightly more accurate values
                    // since it doesn't need to be constructed on the stack.
                    timeval end_time{};
                    timeval elapsed{};
            };
        }
    }
}

//
// Created by permal on 8/1/17.
//

#pragma once

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
                        end_time = std::chrono::steady_clock::now();
                        active = false;
                        elapsed = end_time - start_time;
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
                        start_time = std::chrono::steady_clock::now();
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
                    std::chrono::steady_clock::time_point start_time{};
                    std::chrono::steady_clock::time_point end_time{};
                    std::chrono::steady_clock::duration elapsed{};
            };
        }
    }
}

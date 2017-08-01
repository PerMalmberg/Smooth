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
            class PerfCount
            {
                public:
                    void start()
                    {
                        active = true;
                        gettimeofday(&start_time, nullptr);
                        end_time = start_time;
                    }

                    void stop()
                    {
                        gettimeofday(&end_time, nullptr);
                        active = false;
                        timersub(end_time, start_time, elapsed);
                    }

                    void reset()
                    {
                        // Simply restart the timer.
                        start();
                    }

                    std::chrono::microseconds get_running_time();

                    void timersub(const timeval& a, const timeval& b, timeval& result);

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

//
// Created by permal on 8/1/17.
//

#include <smooth/core/timer/PerfCount.h>

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            void PerfCount::timersub(const timeval& a, const timeval& b, timeval& result)
            {
                result.tv_sec = a.tv_sec - b.tv_sec;
                result.tv_usec = a.tv_usec - b.tv_usec;
                if (result.tv_usec < 0)
                {
                    --result.tv_sec;
                    result.tv_usec += 1000000;
                }
            }

            std::chrono::microseconds PerfCount::get_running_time()
            {
                if( active )
                {
                    // Calculate new elapsed time
                    gettimeofday(&end_time, nullptr);
                    timersub(end_time, start_time, elapsed);
                }

                std::chrono::microseconds us(elapsed.tv_usec);
                us += std::chrono::seconds( elapsed.tv_sec);
                return us;
            }
        }
    }
}

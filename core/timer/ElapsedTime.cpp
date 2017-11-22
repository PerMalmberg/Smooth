//
// Created by permal on 8/1/17.
//

#include <smooth/core/timer/ElapsedTime.h>

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            std::chrono::microseconds ElapsedTime::get_running_time()
            {
                if (active)
                {
                    // Calculate new elapsed time
                    gettimeofday(&end_time, nullptr);
                    timersub(&end_time, &start_time, &elapsed);
                }

                std::chrono::microseconds us(elapsed.tv_usec);
                us += std::chrono::seconds(elapsed.tv_sec);
                return us;
            }

            std::chrono::microseconds ElapsedTime::get_running_time() const
            {
                timeval local_end_time;
                timeval local_elapsed;

                if (active)
                {
                    // Calculate new elapsed time
                    gettimeofday(&local_end_time, nullptr);
                    timersub(&local_end_time, &start_time, &local_elapsed);
                }

                std::chrono::microseconds us(local_elapsed.tv_usec);
                us += std::chrono::seconds(local_elapsed.tv_sec);
                return us;
            }
        }
    }
}

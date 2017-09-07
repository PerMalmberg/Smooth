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
                if( active )
                {
                    // Calculate new elapsed time
                    gettimeofday(&end_time, nullptr);
                    timersub(&end_time, &start_time, &elapsed);
                }

                std::chrono::microseconds us(elapsed.tv_usec);
                us += std::chrono::seconds( elapsed.tv_sec);
                return us;
            }
        }
    }
}

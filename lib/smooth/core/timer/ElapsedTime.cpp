//
// Created by permal on 8/1/17.
//

#include <smooth/core/timer/ElapsedTime.h>

using namespace std::chrono;

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
                    end_time = std::chrono::steady_clock::now();
                    elapsed = end_time - start_time;
                }

                return duration_cast<microseconds>(elapsed);
            }

            std::chrono::microseconds ElapsedTime::get_running_time() const
            {
                steady_clock::duration local_elapsed{};

                if (active)
                {
                    // Calculate new elapsed time
                    local_elapsed = steady_clock::now() - start_time;
                }

                return duration_cast<microseconds>(local_elapsed);
            }
        }
    }
}

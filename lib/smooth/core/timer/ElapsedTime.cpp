/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "smooth/core/timer/ElapsedTime.h"

using namespace std::chrono;

namespace smooth::core::timer
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

// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <smooth/core/timer/ElapsedTime.h>

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

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

#pragma once
#include <chrono>
#include <smooth/core/util/date.h>

namespace smooth::core::sntp
{
	using namespace std::chrono;
	using namespace date;

    /// Event sent when a SNTP sync notification occurs
    class TimeSyncEvent
    {
        public:
			TimeSyncEvent(system_clock::time_point& timePoint)
				: timePoint(timePoint)
			{
			}

			TimeSyncEvent() = default;
			TimeSyncEvent(const TimeSyncEvent&) = default;

			TimeSyncEvent& operator=(const TimeSyncEvent&) = default;

            /// Gets the system_clock::time_point of the SNTP sync notification
            /// \return The SNTP sync notification timeval.
			system_clock::time_point get_timePoint() const
            {
                return timePoint;
            }
        private:
			system_clock::time_point timePoint = system_clock::from_time_t(0);
    };
}
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

#include <string>
#include <chrono>

namespace smooth::core::timer
{
    /// Interface for timers
    class ITimer
    {
        public:
            virtual ~ITimer() = default;

            /// Starts the timer with the already set interval.
            virtual void start() = 0;

            /// Starts the timer with the specified interval.
            /// \param interval The new interval
            virtual void start(std::chrono::milliseconds interval) = 0;

            /// Stops the timer
            virtual void stop() = 0;

            /// Resets the timer, i.e. starts a new interval.
            virtual void reset() = 0;

            /// Gets the ID of the timer.
            /// \return The id.
            virtual int get_id() const = 0;

            /// Gets the name of the timer
            /// \return The name.
            virtual const std::string& get_name() = 0;
    };
}
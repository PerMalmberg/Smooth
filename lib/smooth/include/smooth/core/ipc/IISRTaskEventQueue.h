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

#include "esp_attr.h"
#include <type_traits>

namespace smooth::core::ipc
{
    /// Interface for ISR-safe task queue, see ISRTaskEventQueue fore more details.
    /// \tparam DataType The type of data to send from the ISR.
    template<typename DataType>
    class IISRTaskEventQueue
    {
            static_assert(std::is_default_constructible<DataType>::value, "DataType must be default-constructible");
            static_assert(std::is_trivial<DataType>::value, "DataType must be a trivial type");

        public:
            virtual ~IISRTaskEventQueue()
            {
            }

            /// When called from an ISR, signals the queue to create an event.
            /// \param data The data to attach to the signal.
            IRAM_ATTR virtual void signal(const DataType& data) = 0;
    };
}

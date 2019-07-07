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

#include <cstdint>

namespace smooth::core
{
    // These are the priorities used by the different tasks in the
    // Smooth framework. Keep your application tasks on a lower
    // priority than the framework services to ensure a functioning
    // system.
    const uint32_t APPLICATION_BASE_PRIO = 5;

    const uint32_t TIMER_SERVICE_PRIO = 19;
    const uint32_t SOCKET_DISPATCHER_PRIO = 20;
}

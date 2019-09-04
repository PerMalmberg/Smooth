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

#include <vector>
#include <chrono>
#include <unordered_map>
#include <smooth/core/ipc/Queue.h>
#include <smooth/core/Task.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/network/Wifi.h>

namespace smooth::core
{
    class EarlyInit : public Task
    {
        public:
            EarlyInit(uint32_t priority, const std::chrono::milliseconds& tick_interval);

            ~EarlyInit() override;
    };

    class Application
        : public EarlyInit
    {
        public:
            /// Constructor
            /// \param priority The priority to run at. Usually smooth::core::APPLICATION_BASE_PRIO
            /// + an arbitrary value, but should be lower than the priority of the ESP-IDFs task
            /// such as the Wifi driver.
            /// \param tick_interval The tick interval
            Application(uint32_t priority, std::chrono::milliseconds tick_interval)
                    : EarlyInit(priority, tick_interval)
            {
            }

            void init() override;

            network::Wifi& get_wifi()
            {
                return wifi;
            }

        private:
            network::Wifi wifi{};
    };
}

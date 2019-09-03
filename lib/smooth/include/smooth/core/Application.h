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
    /// The Application 'attaches' itself to the main task and gives the application programmer
    /// the same possibilities to perform work on the main task as if a separate Task had been created.
    /// The Application class is also responsible for hooking the system event queue and distributing
    /// those events. Any application written based on Smooth should have an instance of the Application
    /// class (or a class derived from Application) on the stack in its app_main().
    /// Be sure to adjust the stack size of the main task accordingly using 'make menuconfig'.
    /// Note: Unlike the version of start() in Task, when called on an Application instance start() never returns.
    class POSIXApplication
        : public Task
    {
        public:
            /// Constructor
            /// \param priority The priority to run at. Usually tskIDLE_PRIORITY + an arbitrary value,
            /// but should be lower than the priority of the ESP-IDFs task such as the Wifi driver.
            /// \param tick_interval The tick interval
            POSIXApplication(uint32_t priority, std::chrono::milliseconds tick_interval);

            /// Initialize the application.
            void init() override;

            /// Returns the Wifi manager
            /// \return The Wifi management instance
            network::Wifi& get_wifi()
            {
                return wifi;
            }

        private:
            network::Wifi wifi{};
    };

#ifdef ESP_PLATFORM

    /// The IDFApplication extends Application with things needed to run under the IDF framework
    class IDFApplication
        : public POSIXApplication
    {
        public:
            /// Constructor
            /// \param priority The priority to run at. Usually tskIDLE_PRIORITY + an arbitrary value,
            /// but should be lower than the priority of the ESP-IDFs task such as the Wifi driver.
            /// \param tick_interval The tick interval
            IDFApplication(uint32_t priority, std::chrono::milliseconds tick_interval);

            ~IDFApplication() override = default;

        protected:

            void init() override;

        private:

            static void wifi_event_callback(void* event_handler_arg,
                                            esp_event_base_t event_base,
                                            int32_t event_id,
                                            void* event_data);
    };

#endif // END ESP_PLATFORM

    class Application
        :
#ifdef ESP_PLATFORM
        public IDFApplication
#else
        public POSIXApplication
#endif
    {
        public:
            /// Constructor
            /// \param priority The priority to run at. Usually tskIDLE_PRIORITY + an arbitrary value,
            /// but should be lower than the priority of the ESP-IDFs task such as the Wifi driver.
            /// \param tick_interval The tick interval
            Application(uint32_t priority, std::chrono::milliseconds tick_interval)
                    :
#ifdef ESP_PLATFORM
                      IDFApplication(priority, tick_interval)
#else
                      POSIXApplication(priority, tick_interval)
#endif
            {
            }
    };
}

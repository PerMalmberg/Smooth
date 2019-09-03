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

#include <smooth/core/Application.h>
#include <smooth/core/network/SocketDispatcher.h>
#include <smooth/core/filesystem/FSLock.h>
#include <smooth/core/ipc/Publisher.h>

#ifdef ESP_PLATFORM
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <smooth/core/logging/log.h>
#endif // END ESP_PLATFORM

using namespace smooth::core::filesystem;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace smooth::core
{
    POSIXApplication::POSIXApplication(uint32_t priority, std::chrono::milliseconds tick_interval)
            : Task(priority, tick_interval)
    {
        // Set the number of allowed open files to something that might be used when running on the ESP.
        FSLock::init(5);
    }

    void POSIXApplication::init()
    {
        // Start socket dispatcher first of all so that it is
        // ready to receive network status events.
        network::SocketDispatcher::instance();
#ifndef ESP_PLATFORM

        // Assume network is available when running under POSIX system.
        network::NetworkStatus status(network::NetworkEvent::GOT_IP, true);
        core::ipc::Publisher<network::NetworkStatus>::publish(status);
#endif
    }

#ifdef ESP_PLATFORM
    IDFApplication::IDFApplication(uint32_t priority, std::chrono::milliseconds tick_interval)
            : POSIXApplication(priority, tick_interval)
    {
        nvs_flash_init();
        gpio_install_isr_service(0);
        //esp_event_loop_create_default(); // QQQ
    }

    void IDFApplication::init()
    {
        // Start socket dispatcher first of all so that it is
        // ready to receive network status events.
        network::SocketDispatcher::instance();

        if (get_wifi().is_configured())
        {
            get_wifi().connect_to_ap();
        }
    }

#endif
}

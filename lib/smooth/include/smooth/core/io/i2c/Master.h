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

#include <memory>
#include <mutex>
#include <driver/i2c.h>
#include <driver/gpio.h>

namespace smooth::core::io::i2c
{
    class Master
    {
        public:
            Master(i2c_port_t port,
                   gpio_num_t scl,
                   bool slc_internal_pullup_enable,
                   gpio_num_t sda,
                   bool sda_internal_pullup_enable,
                   uint32_t clock_frequency_hz);

            ~Master();

            bool initialize();

            /// Creates a device of the given type.
            /// \tparam DeviceType The device type.
            /// \tparam Args The device-specific argument types.
            /// \param address The device address.
            /// \param args The device-specific arguments
            /// \return A device, or nullptr.
            template<typename DeviceType, typename ...Args>
            std::unique_ptr<DeviceType> create_device(uint8_t address, Args&& ...args);

        private:
            void do_initialization();

            void deinitialize();

            bool initialized = false;

            // This mutex is shared among all the devices created from this master.
            std::mutex guard{};
            i2c_config_t config{};
            i2c_port_t port;
    };

    template<typename DeviceType, typename ...Args>
    std::unique_ptr<DeviceType> Master::create_device(uint8_t address, Args&& ...args)
    {
        std::unique_ptr<DeviceType> dev;
        if (initialize())
        {
            std::lock_guard<std::mutex> lock(guard);
            dev = std::make_unique<DeviceType>(port, address, guard, std::forward<Args>(args)...);
        }
        return dev;
    }
}


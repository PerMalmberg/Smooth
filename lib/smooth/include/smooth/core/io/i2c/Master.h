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

#pragma once

#include <memory>
#include <mutex>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <driver/i2c.h>
#include <driver/gpio.h>
#pragma GCC diagnostic pop

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
            template<typename DeviceType, typename... Args>
            std::unique_ptr<DeviceType> create_device(uint8_t address, Args&& ... args);

        private:
            void do_initialization();

            void deinitialize();

            bool initialized = false;

            // This mutex is shared among all the devices created from this master.
            std::mutex guard{};
            i2c_config_t config{};
            i2c_port_t port;
    };

    template<typename DeviceType, typename... Args>
    std::unique_ptr<DeviceType> Master::create_device(uint8_t address, Args&& ... args)
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

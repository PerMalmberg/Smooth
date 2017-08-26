//
// Created by permal on 8/19/17.
//

#pragma once

#include <memory>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include <smooth/core/ipc/Mutex.h>
#include <smooth/core/util/make_unique.h>

namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace i2c
            {
                class Master
                {
                    public:
                        Master(i2c_port_t port,
                               gpio_num_t slc,
                               bool slc_internal_pullup_enable,
                               gpio_num_t sda,
                               bool sda_internal_pullup_enable,
                               int clock_frequency_hz);

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
                        core::ipc::Mutex guard{};
                        i2c_config_t config{};
                        i2c_port_t port;
                };

                template<typename DeviceType, typename ...Args>
                std::unique_ptr<DeviceType> Master::create_device(uint8_t address, Args&& ...args)
                {
                    std::unique_ptr<DeviceType> dev;
                    if (initialize())
                    {
                        ipc::Mutex::Lock lock(guard);
                        dev = core::util::make_unique<DeviceType>(port, address, guard, std::forward<Args>(args)...);
                    }
                    return dev;
                }

            }
        }
    }
}

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

                        template<typename DeviceType, typename ...Args>
                        std::unique_ptr<DeviceType> add_device(Args&& ...args);

                    private:
                        bool initialized = false;
                        core::ipc::Mutex guard{};
                        i2c_config_t config{};
                        i2c_port_t port;
                };

                template<typename DeviceType, typename ...Args>
                std::unique_ptr<DeviceType> Master::add_device(Args&& ...args)
                {
                    std::unique_ptr<DeviceType> dev;
                    if (initialize())
                    {
                        ipc::Mutex::Lock lock(guard);
                        dev = core::util::make_unique<DeviceType>(port, guard, std::forward<Args>(args)...);
                    }
                    return dev;
                }

            }
        }
    }
}

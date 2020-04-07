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

#include "smooth/core/io/i2c/Master.h"
#include "smooth/core/logging/log.h"
#include <driver/gpio.h>

using namespace smooth::core::logging;

namespace smooth::core::io::i2c
{
    const char* log_tag = "I2CMaster";

    Master::Master(i2c_port_t port,
                   gpio_num_t scl,
                   bool scl_internal_pullup_enable,
                   gpio_num_t sda,
                   bool sda_internal_pullup_enable,
                   uint32_t clock_frequency_hz)
            : port(port)
    {
        config.mode = I2C_MODE_MASTER;
        config.scl_io_num = scl;
        config.sda_io_num = sda;
        config.scl_pullup_en = scl_internal_pullup_enable ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
        config.sda_pullup_en = sda_internal_pullup_enable ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
        config.master.clk_speed = clock_frequency_hz;
    }

    Master::~Master()
    {
        deinitialize();
    }

    bool Master::initialize()
    {
        std::lock_guard<std::mutex> lock(guard);
        do_initialization();

        return initialized;
    }

    void Master::do_initialization()
    {
        if (!initialized)
        {
            initialized = i2c_param_config(port, &config) == ESP_OK
                          && i2c_driver_install(port, config.mode, 0, 0, ESP_INTR_FLAG_LOWMED) == ESP_OK;

            if (!initialized)
            {
                Log::error(log_tag, "Initialization failed");
            }
        }
    }

    void Master::deinitialize()
    {
        if (initialized)
        {
            initialized = false;
            i2c_driver_delete(port);
        }
    }
}

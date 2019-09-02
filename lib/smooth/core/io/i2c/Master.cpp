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

#include <smooth/core/io/i2c/Master.h>
#include <smooth/core/logging/log.h>
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
                Log::error(log_tag, Format("Initialization failed"));
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

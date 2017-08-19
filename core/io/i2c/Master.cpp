//
// Created by permal on 8/19/17.
//

#include <smooth/core/io/i2c/Master.h>
#include "esp_log.h"
#include "esp_intr_alloc.h"

namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace i2c
            {
                const char* log_tag = "I2CMaster";

                Master::Master(i2c_port_t port,
                               gpio_num_t slc,
                               bool slc_internal_pullup_enable,
                               gpio_num_t sda,
                               bool sda_internal_pullup_enable,
                               int clock_frequency_hz)
                        : port(port)
                {
                    config.mode = I2C_MODE_MASTER;
                    config.scl_io_num = slc;
                    config.sda_io_num = sda;
                    config.scl_pullup_en = slc_internal_pullup_enable ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
                    config.sda_pullup_en = sda_internal_pullup_enable ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
                    config.master.clk_speed = clock_frequency_hz;
                }

                Master::~Master()
                {
                    if (initialized)
                    {
                        i2c_driver_delete(port);
                    }
                }

                bool Master::initialize()
                {
                    ipc::Mutex::Lock lock(guard);

                    if (!initialized)
                    {
                        initialized = i2c_param_config(port, &config) == ESP_OK
                                      && i2c_driver_install(port, config.mode, 0, 0, ESP_INTR_FLAG_LOWMED) == ESP_OK;

                        if (!initialized)
                        {
                            ESP_LOGE(log_tag, "Initialization failed");
                        }
                    }

                    return initialized;
                }
            }
        }
    }
}
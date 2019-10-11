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

#include <smooth/core/io/spi/Master.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

namespace smooth::core::io::spi
{
    Master::Master(
        spi_host_device_t host,
        SPI_DMA_Channel dma_channel,
        gpio_num_t mosi,
        gpio_num_t miso,
        gpio_num_t clock,
        gpio_num_t quadwp_io_num,
        gpio_num_t quadhd_io_num
        )
            : host(host),
              dma_channel(dma_channel)
    {
        bus_config.mosi_io_num = mosi;
        bus_config.miso_io_num = miso;
        bus_config.sclk_io_num = clock;

        bus_config.quadwp_io_num = quadwp_io_num;
        bus_config.quadhd_io_num = quadhd_io_num;
    }

    bool Master::initialize()
    {
        std::lock_guard<std::mutex> lock(guard);

        if (!initialized)
        {
            esp_err_t res = spi_bus_initialize(host, &bus_config, dma_channel);

            if (res == ESP_ERR_INVALID_ARG)
            {
                Log::error(log_tag, "Invalid configuration");
            }
            else if (res == ESP_ERR_INVALID_STATE)
            {
                Log::error(log_tag, "Host already is in use");
            }
            else if (res == ESP_ERR_NO_MEM)
            {
                Log::error(log_tag, "Out of memory");
            }
            else
            {
                Log::verbose(log_tag, "SPI initialized, Host {}, DMA {}", host, dma_channel);
            }

            initialized = res == ESP_OK;
        }

        return initialized;
    }
}

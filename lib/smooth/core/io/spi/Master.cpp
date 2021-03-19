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
#include "smooth/core/io/spi/Master.h"
#include "smooth/core/logging/log.h"

using namespace smooth::core::logging;

namespace smooth::core::io::spi
{
    static constexpr const char* log_tag = "SPIMaster";

    // Declare static variables
    bool Master::spi_bus_initialized = false;
    bool Master::hspi_initialized = false;
    bool Master::vspi_initialized = false;
    std::mutex Master::guard{};
    spi_bus_config_t Master::bus_config{};
    spi_host_device_t Master::spi_host;
    SPI_DMA_Channel Master::dma_channel;

    bool Master::initialize(spi_host_device_t host,
                            SPI_DMA_Channel dma_chl,
                            gpio_num_t mosi,
                            gpio_num_t miso,
                            gpio_num_t clock,
                            int transfer_size,
                            gpio_num_t quadwp_io_num,
                            gpio_num_t quadhd_io_num)
    {
        std::lock_guard<std::mutex> lock(guard);

        spi_host = host;
        dma_channel = dma_chl;

        bus_config.mosi_io_num = mosi;
        bus_config.miso_io_num = miso;
        bus_config.sclk_io_num = clock;
        bus_config.quadwp_io_num = quadwp_io_num;
        bus_config.quadhd_io_num = quadhd_io_num;
        bus_config.max_transfer_sz = transfer_size;

        spi_bus_initialized = false;

        if (spi_host == VSPI_HOST)
        {
            spi_bus_initialized = do_vspi_initialization();
        }

        if (spi_host == HSPI_HOST)
        {
            spi_bus_initialized = do_hspi_initialization();
        }

        return spi_bus_initialized;
    }

    bool Master::do_vspi_initialization()
    {
        if (!vspi_initialized)
        {
            vspi_initialized = spi_bus_initialize(spi_host, &bus_config, dma_channel) == ESP_OK;

            if (!vspi_initialized)
            {
                Log::error(log_tag, "VSPI_HOST Initialization failed");
            }
            else
            {
                Log::info(log_tag, "VSPI initialized, Host {}, DMA {}", spi_host, dma_channel);
            }
        }

        return vspi_initialized;
    }

    bool Master::do_hspi_initialization()
    {
        if (!hspi_initialized)
        {
            hspi_initialized = spi_bus_initialize(spi_host, &bus_config, dma_channel) == ESP_OK;

            if (!hspi_initialized)
            {
                Log::error(log_tag, "HSPI_HOST Initialization failed");
            }
            else
            {
                Log::info(log_tag, "HSPI initialized, Host {}, DMA {}", spi_host, dma_channel);
            }
        }

        return hspi_initialized;
    }

    void Master::deinitialize(spi_host_device_t spi_host)
    {
        if ((spi_host == VSPI_HOST) & (vspi_initialized))
        {
            vspi_initialized = false;
            spi_bus_free(VSPI_HOST);
        }

        if ((spi_host == HSPI_HOST) & (hspi_initialized))
        {
            hspi_initialized = false;
            spi_bus_free(HSPI_HOST);
        }
    }
}

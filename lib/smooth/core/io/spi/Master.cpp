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

    Master::Master(
        spi_host_device_t host,
        SPI_DMA_Channel dma_channel,
        gpio_num_t mosi,
        gpio_num_t miso,
        gpio_num_t clock,
        int transfer_size,
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
        bus_config.max_transfer_sz = transfer_size;
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
            initialized = spi_bus_initialize(host, &bus_config, dma_channel) == ESP_OK;

            if (!initialized)
            {
                Log::error(log_tag, "Initialization failed");
            }
            else
            {
                Log::verbose(log_tag, "SPI initialized, Host {}, DMA {}", host, dma_channel);
            }
        }
    }

    void Master::deinitialize()
    {
        if (initialized)
        {
            initialized = false;
            spi_bus_free(host);
        }
    }
}

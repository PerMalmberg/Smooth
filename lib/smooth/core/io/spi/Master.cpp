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
    static constexpr const char* spi2_host_str = "SPI2_HOST";
    static constexpr const char* spi3_host_str = "SPI3_HOST";

    // Declare static variables
    bool Master::spi2_initialized = false;
    bool Master::spi3_initialized = false;
    std::mutex Master::guard{};
    spi_bus_config_t Master::bus_config{};
    spi_host_device_t Master::spi_host;
    SPI_DMA_Channel Master::dma_channel;
    uint8_t Master::spi2_initialized_count = 0;
    uint8_t Master::spi3_initialized_count = 0;

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

        bool initialized = false;

        if (spi_host == SPI2_HOST)
        {
            initialized = do_intitialization(SPI2_HOST, spi2_initialized, spi2_initialized_count, spi2_host_str);
        }

        if (spi_host == SPI3_HOST)
        {
            initialized = do_intitialization(SPI3_HOST, spi3_initialized, spi3_initialized_count, spi3_host_str);
        }

        return initialized;
    }

    bool Master::do_intitialization(spi_host_device_t host,
                                    bool& initialized,
                                    uint8_t& initialized_count,
                                    const char* spi_host_str)
    {
        if (!initialized)
        {
            initialized = spi_bus_initialize(host, &bus_config, dma_channel) == ESP_OK;

            if (!initialized)
            {
                Log::error(log_tag, "{} Initialization has failed", spi_host_str);
            }
            else
            {
                Log::info(log_tag, "{} has been initialized using DMA channel {}", spi_host_str, dma_channel);
            }
        }

        // if initialized then increment initialized_count so we know how many spi-devices are on this spi-bus
        if (initialized)
        {
            initialized_count++;
        }

        Log::verbose(log_tag, "{} has an initialized_count = {}", spi_host_str, initialized_count);

        return initialized;
    }

    void Master::deinitialize(spi_host_device_t spi_host)
    {
        std::lock_guard<std::mutex> lock(guard);

        if (spi_host == SPI2_HOST)
        {
            do_deinitialize(SPI2_HOST, spi2_initialized, spi2_initialized_count, spi2_host_str);
        }

        if (spi_host == SPI3_HOST)
        {
            do_deinitialize(SPI3_HOST, spi3_initialized, spi3_initialized_count, spi3_host_str);
        }
    }

    void Master::do_deinitialize(spi_host_device_t host,
                                 bool& initialized,
                                 uint8_t& initialized_count,
                                 const char* spi_host_str)
    {
        if (initialized)
        {
            initialized_count--;

            Log::verbose(log_tag, "{} has an initialized_count = {}", spi_host_str, initialized_count);

            // check to see if this is the last spi device on this spi-bus, if true free this spi-bus
            if (initialized_count == 0)
            {
                initialized = false;
                auto res = spi_bus_free(host);

                if (res == ESP_OK)
                {
                    Log::info(log_tag, "{} has successfully been freed", spi_host_str);
                }
                else
                {
                    Log::error(log_tag, "spi_bus_free for {} has failed", spi_host_str);
                }
            }
        }
    }
}

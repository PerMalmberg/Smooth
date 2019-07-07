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
                Log::error(log_tag, Format("Invalid configuration"));
            }
            else if (res == ESP_ERR_INVALID_STATE)
            {
                Log::error(log_tag, Format("Host already is in use"));
            }
            else if (res == ESP_ERR_NO_MEM)
            {
                Log::error(log_tag, Format("Out of memory"));
            }
            else
            {
                Log::verbose(log_tag, Format("SPI initialized, Host {1}, DMA {2}", Int32(host), Int32(dma_channel)));
            }

            initialized = res == ESP_OK;
        }

        return initialized;
    }
}

//
// Created by permal on 8/18/17.
//

#include <smooth/core/io/spi/Master.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace spi
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
        }
    }
}
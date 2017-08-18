//
// Created by permal on 8/18/17.
//

#include <smooth/core/io/spi/Master.h>
#include "esp_log.h"

namespace smooth
{
    namespace core
    {
        namespace io
        {
            namespace spi
            {
                static const char* log_tag = "SPIMaster";

                Master::Master(
                        spi_host_device_t host,
                        SPI_DMA_Channel dma_channel,
                        gpio_num_t mosi,
                        gpio_num_t miso,
                        gpio_num_t clock,
                        gpio_num_t chip_select,
                        gpio_num_t data__or_command)
                        : host(host),
                          dma_channel(dma_channel)
                {
                    bus_config.mosi_io_num = mosi;
                    bus_config.miso_io_num = miso;
                    bus_config.sclk_io_num = clock;

                    // This implementation only supports four-wire communications.
                    bus_config.quadwp_io_num = -1;
                    bus_config.quadhd_io_num = -1;
                }

                bool Master::initialize() const
                {
                    esp_err_t res = spi_bus_initialize(host, &bus_config, dma_channel);

                    if (res == ESP_ERR_INVALID_ARG)
                    {
                        ESP_LOGE(log_tag, "Invalid configuration");
                    }
                    else if (res == ESP_ERR_INVALID_STATE)
                    {
                        ESP_LOGE(log_tag, "Host already is in use");
                    }
                    else if (res == ESP_ERR_NO_MEM)
                    {
                        ESP_LOGE(log_tag, "Out of memory");
                    }
                    else
                    {
                        ESP_LOGV(log_tag, "SPI initialized, Host %d, DMA %d", host, dma_channel);
                    }

                    return res == ESP_OK;
                }
            }
        }
    }
}
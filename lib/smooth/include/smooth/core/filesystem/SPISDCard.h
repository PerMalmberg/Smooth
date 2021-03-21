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

#pragma once

#include <driver/gpio.h>
#include "SDCard.h"
#include "MountPoint.h"
#include "smooth/core/io/spi/Master.h"

namespace smooth::core::filesystem
{
    class SPISDCard
        : public SDCard
    {
        public:
            SPISDCard(spi_host_device_t spi_host,
                      smooth::core::io::spi::SPI_DMA_Channel dma_channel,
                      gpio_num_t miso,
                      gpio_num_t mosi,
                      gpio_num_t serial_clock,
                      gpio_num_t chip_select,
                      gpio_num_t card_detect = GPIO_NUM_NC,
                      gpio_num_t write_protect = GPIO_NUM_NC,
                      int transfer_size = 0);

            bool init(const SDCardMount& mount_point, bool format_on_mount_failure, int max_file_count) override;

            bool deinit() override;

        private:
            spi_host_device_t spi_host;
            smooth::core::io::spi::SPI_DMA_Channel dma_chl;
            gpio_num_t miso;
            gpio_num_t mosi;
            gpio_num_t serial_clock;
            gpio_num_t chip_select;
            gpio_num_t card_detect;
            gpio_num_t write_protect;
            int transfer_size;
    };
}

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

#include "smooth/core/filesystem/SPISDCard.h"
#include "smooth/core/filesystem/FSLock.h"

using namespace smooth::core::io::spi;

namespace smooth::core::filesystem
{
    SPISDCard::SPISDCard(spi_host_device_t spi_host,
                         SPI_DMA_Channel dma_channel,
                         gpio_num_t miso,
                         gpio_num_t mosi,
                         gpio_num_t serial_clock,
                         gpio_num_t chip_select,
                         gpio_num_t card_detect,
                         gpio_num_t write_protect,
                         int transfer_size)
            : spi_host(spi_host),
              dma_chl(dma_channel),
              miso(miso),
              mosi(mosi),
              serial_clock(serial_clock),
              chip_select(chip_select),
              card_detect(card_detect),
              write_protect(write_protect),
              transfer_size(transfer_size)
    {
    }

    bool SPISDCard::init(const SDCardMount& mount_point, bool format_on_mount_failure, int max_file_count)
    {
        Master::initialize(spi_host,
                           dma_chl,
                           mosi,
                           miso,
                           serial_clock);

        esp_vfs_fat_sdmmc_mount_config_t mount_config{};
        mount_config.format_if_mount_failed = format_on_mount_failure;
        mount_config.max_files = max_file_count;

        sdmmc_host = (sdmmc_host_t)SDSPI_HOST_DEFAULT();
        sdmmc_host.slot = spi_host;

        sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
        slot_config.gpio_cs = chip_select;
        slot_config.host_id = static_cast<spi_host_device_t>(sdmmc_host.slot);

        if (card_detect != -1)
        {
            slot_config.gpio_cd = card_detect;
        }

        if (write_protect != -1)
        {
            slot_config.gpio_wp = write_protect;
        }

        auto mount_result = esp_vfs_fat_sdspi_mount((*mount_point).c_str(),
                                                     &sdmmc_host,
                                                     &slot_config,
                                                     &mount_config,
                                                     &card);

        FSLock::set_limit(max_file_count);

        return do_common_error_checking(mount_result);
    }

    bool SPISDCard::deinit()
    {
        auto res = SDCard::deinit();
        Master::deinitialize(spi_host);

        return res;
    }
}

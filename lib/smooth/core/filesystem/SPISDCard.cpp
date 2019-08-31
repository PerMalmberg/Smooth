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

#include <smooth/core/filesystem/SDCard.h>
#include <smooth/core/filesystem/SPISDCard.h>
#include <driver/sdspi_host.h>

namespace smooth::core::filesystem
{
    SPISDCard::SPISDCard(gpio_num_t miso,
                         gpio_num_t mosi,
                         gpio_num_t serial_clock,
                         gpio_num_t chip_select,
                         gpio_num_t card_detect,
                         gpio_num_t write_protect)
            : miso(miso),
              mosi(mosi),
              serial_clock(serial_clock),
              chip_select(chip_select),
              card_detect(card_detect),
              write_protect(write_protect)
    {
    }

    bool SPISDCard::init(const SDCardMount& mount_point, bool format_on_mount_failure, int max_file_count)
    {
        host = SDSPI_HOST_DEFAULT();
        sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();

        slot_config.gpio_miso = miso;
        slot_config.gpio_mosi = mosi;
        slot_config.gpio_sck = serial_clock;
        slot_config.gpio_cs = chip_select;

        if (card_detect != -1)
        {
            slot_config.gpio_cd = card_detect;
        }

        if (write_protect != -1)
        {
            slot_config.gpio_wp = write_protect;
        }

        return do_common_initialization(mount_point, max_file_count, format_on_mount_failure, &slot_config);
    }
}

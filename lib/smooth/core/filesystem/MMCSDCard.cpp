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

#include "smooth/core/filesystem/MMCSDCard.h"

namespace smooth::core::filesystem
{
    MMCSDCard::MMCSDCard(gpio_num_t command,
                         gpio_num_t data0,
                         gpio_num_t data1,
                         gpio_num_t data2,
                         gpio_num_t data3,
                         bool use_1_line_mode,
                         gpio_num_t card_detect,
                         gpio_num_t write_protect)
            : command(command),
              data0(data0),
              data1(data1),
              data2(data2),
              data3(data3),
              use_1_line_mode(use_1_line_mode),
              card_detect(card_detect),
              write_protect(write_protect)
    {
    }

    bool MMCSDCard::init(const SDCardMount& mount_point, bool format_on_mount_failure, int max_file_count)
    {
        host = SDMMC_HOST_DEFAULT();
        // QQQ host.max_freq_khz = SDMMC_FREQ_HIGHSPEED()

        sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

        if (card_detect != -1)
        {
            slot_config.gpio_cd = card_detect;
        }

        if (write_protect != -1)
        {
            slot_config.gpio_wp = write_protect;
        }

        if (use_1_line_mode)
        {
            slot_config.width = 1;
        }

        gpio_set_pull_mode(command, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
        gpio_set_pull_mode(data0, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
        gpio_set_pull_mode(data1, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
        gpio_set_pull_mode(data2, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
        gpio_set_pull_mode(data3, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes

        return do_common_initialization(mount_point, max_file_count, format_on_mount_failure, &slot_config);
    }
}
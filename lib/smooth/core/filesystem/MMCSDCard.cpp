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
        host = (sdmmc_host_t)SDMMC_HOST_DEFAULT();

        // host.max_freq_khz = SDMMC_FREQ_HIGHSPEED()

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

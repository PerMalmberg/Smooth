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

#pragma once

#include "SDCard.h"
#include <driver/gpio.h>

namespace smooth::core::filesystem
{
    class SPISDCard
            : public SDCard
    {
        public:
            SPISDCard(gpio_num_t miso,
                      gpio_num_t mosi,
                      gpio_num_t serial_clock,
                      gpio_num_t chip_select,
                      gpio_num_t card_detect = GPIO_NUM_NC,
                      gpio_num_t write_protect = GPIO_NUM_NC);

            bool init(const char* mount_point, bool format_on_mount_failure, int max_file_count) override;

        private:
            gpio_num_t miso;
            gpio_num_t mosi;
            gpio_num_t serial_clock;
            gpio_num_t chip_select;
            gpio_num_t card_detect;
            gpio_num_t write_protect;
            sdmmc_card_t* card;
    };
}

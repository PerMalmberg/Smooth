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
#include <smooth/core/io/mock/gpio.h>

namespace smooth::core::filesystem
{
    class MMCSDCard
            : public SDCard
    {
        public:
            MMCSDCard(gpio_num_t /*command*/,
                      gpio_num_t /*data0*/,
                      gpio_num_t /*data1*/,
                      gpio_num_t /*data2*/,
                      gpio_num_t /*data3*/,
                      bool use_1_line_mode = false,
                      gpio_num_t card_detect = static_cast<gpio_num_t>(-1),
                      gpio_num_t write_protect = static_cast<gpio_num_t>(-1))
                      :SDCard()
            {
                (void)use_1_line_mode;
                (void)card_detect;
                (void)write_protect;
            }
    };
}

//
// Created by permal on 2018-10-27.
//

#pragma once

#include "SDCard.h"
#include <driver/gpio.h>

namespace smooth
{
    namespace core
    {
        namespace filesystem
        {
            class SPISDCard : public SDCard {
                public:
                    SPISDCard(gpio_num_t miso,
                              gpio_num_t mosi,
                              gpio_num_t serial_clock,
                              gpio_num_t chip_select,
                              gpio_num_t card_detect = static_cast<gpio_num_t>(-1),
                              gpio_num_t write_protect = static_cast<gpio_num_t>(-1));

                    bool init(const char *mount_point, bool format_on_mount_failure, int max_file_count) override;
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
    }
}



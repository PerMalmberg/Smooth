//
// Created by permal on 2018-10-27.
//

#pragma once

#include "SDCard.h"
#include "driver/sdmmc_host.h"

namespace smooth
{
    namespace core
    {
        namespace filesystem
        {
            class MMCSDCard
                    : public SDCard
            {
                public:
                    MMCSDCard(gpio_num_t command,
                              gpio_num_t data0,
                              gpio_num_t data1,
                              gpio_num_t data2,
                              gpio_num_t data3,
                              bool use_1_line_mode = false,
                              gpio_num_t card_detect = static_cast<gpio_num_t>(-1),
                              gpio_num_t write_protect = static_cast<gpio_num_t>(-1));

                    bool init(const char *mount_point, bool format_on_mount_failure, int max_file_count) override;

                private:
                    gpio_num_t command;
                    gpio_num_t data0;
                    gpio_num_t data1;
                    gpio_num_t data2;
                    gpio_num_t data3;
                    bool use_1_line_mode;
                    gpio_num_t card_detect;
                    gpio_num_t write_protect;
            };
        }
    }
}

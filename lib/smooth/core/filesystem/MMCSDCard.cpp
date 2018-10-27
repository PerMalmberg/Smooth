//
// Created by permal on 2018-10-27.
//

#include "smooth/core/filesystem/MMCSDCard.h"

namespace smooth
{
    namespace core
    {
        namespace filesystem
        {
            MMCSDCard::MMCSDCard(gpio_num_t command,
                                 gpio_num_t data0,
                                 gpio_num_t data1,
                                 gpio_num_t data2,
                                 gpio_num_t data3,
                                 bool enable_4_line_mode,
                                 gpio_num_t card_detect,
                                 gpio_num_t write_protect)
                    : command(command),
                      data0(data0),
                      data1(data1),
                      data2(data2),
                      data3(data3),
                      enable_4_line_mode(enable_4_line_mode),
                      card_detect(card_detect),
                      write_protect(write_protect)
            {
                // QQQ host.max_freq_khz = SDMMC_FREQ_HIGHSPEED()
            }
        }
    }
}
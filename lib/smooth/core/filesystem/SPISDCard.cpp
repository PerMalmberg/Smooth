//
// Created by permal on 2018-10-27.
//

#include <smooth/core/filesystem/SDCard.h>
#include <smooth/core/filesystem/SPISDCard.h>
#include <driver/sdspi_host.h>

namespace smooth
{
    namespace core
    {
        namespace filesystem
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

            bool SPISDCard::init(const char* mount_point, bool format_on_mount_failure, int max_file_count)
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
    }
}
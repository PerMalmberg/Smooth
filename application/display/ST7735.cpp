//
// Created by permal on 8/18/17.
//

#include <chrono>
#include <smooth/application/display/ST7735.h>
#include <smooth/core/Task.h>

using namespace smooth::core::io::spi;

namespace smooth
{
    namespace application
    {
        namespace display
        {
            ST7735::ST7735(core::io::spi::Master& master,
                           gpio_num_t chip_select_pin,
                           gpio_num_t data_command_pin,
                           gpio_num_t reset_pin,
                           gpio_num_t back_light_pin)
                    : master(master),
                      reset_pin(reset_pin, false),
                      back_light(back_light_pin, true),
                      chip_select_pin(chip_select_pin),
                      data_command_pin(data_command_pin)
            {
            }

            bool ST7735::initialize()
            {
                auto res = master.initialize();
                if (res)
                {
                    spi = std::move(master.add_device<ST7735_SPI>(chip_select_pin, data_command_pin));
                }

                if (spi)
                {
                    // Reset the display
                    reset_pin.set();
                    core::Task::delay(std::chrono::milliseconds(1));
                    reset_pin.clr();
                    // Wait for reset completion
                    core::Task::delay(std::chrono::milliseconds(120));
                }

                return spi ? true : false;
            }
        }
    }
}

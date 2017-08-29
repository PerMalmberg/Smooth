//
// Created by permal on 8/18/17.
//

#include <cstring>
#include <chrono>
#include <smooth/application/display/ST7735.h>
#include <smooth/core/Task.h>
#include <smooth/core/ipc/Mutex.h>


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
                      reset_pin(reset_pin, false, true, false),
                      back_light(back_light_pin, true, false, true),
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

            void ST7735::read_display_id(std::array<uint8_t, 4> id)
            {

            }

            void ST7735::software_reset()
            {
                core::ipc::Mutex::Lock lock(spi->get_guard());
                spi_transaction_t trans;
                std::memset(&trans, 0, sizeof(trans));
                trans.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
                trans.length = 8 * 1;
                trans.tx_data[0] = 0x01;
                spi->write(trans);
            }
        }
    }
}

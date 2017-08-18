//
// Created by permal on 8/18/17.
//

#include <smooth/application/display/ST7735.h>

using namespace smooth::core::io::spi;

namespace smooth
{
    namespace application
    {
        namespace display
        {
            ST7735::ST7735(core::io::spi::Master& master)
                    : master(master)
            {
            }

            bool ST7735::initialize()
            {
                auto res = master.initialize();
                if (res)
                {
                    spi = std::move(master.add_device<ST7735_SPI>(GPIO_NUM_22, GPIO_NUM_21));
                }

                return spi ? true:false;
            }
        }
    }
}

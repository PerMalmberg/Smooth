#include "sdcard_spi.h"
#include <smooth/core/filesystem/SPISDCard.h>

#include <smooth/core/logging/log.h>
#include <smooth/core/task_priorities.h>


using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace sdcard_spi
{

    App::App()
            : Application(APPLICATION_BASE_PRIO,
                          std::chrono::milliseconds(1000))
    {
    }

    void App::init()
    {
        Application::init();

        // Change the pin configuration to match your hardware.
        card = std::make_unique<smooth::core::filesystem::SPISDCard>(GPIO_NUM_19,
                                                                     GPIO_NUM_23,
                                                                     GPIO_NUM_18,
                                                                     GPIO_NUM_5,
                                                                     GPIO_NUM_21);

    }

    void App::tick()
    {
        if (!card->is_initialized())
        {
            card->init("/sdcard", false, 5);
        }
    }
}
#include "sdcard_mmc.h"
#include <smooth/core/filesystem/MMCSDCard.h>

#include <smooth/core/logging/log.h>
#include <smooth/core/task_priorities.h>
#include <fstream>


using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace sdcard_mmc
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
        card = std::make_unique<smooth::core::filesystem::MMCSDCard>(GPIO_NUM_15,
                                                                     GPIO_NUM_2,
                                                                     GPIO_NUM_26,
                                                                     GPIO_NUM_12,
                                                                     GPIO_NUM_13,
                                                                     false,
                                                                     GPIO_NUM_21);

    }

    void App::tick()
    {
        if (!card->is_initialized())
        {
            if (card->init("/sdcard", false, 5))
            {
                std::ofstream f{"/sdcard/mmc.txt", std::ios::out};
                if (f.good())
                {
                    f << "MMC Initialized";
                }
            }
        }
    }
}
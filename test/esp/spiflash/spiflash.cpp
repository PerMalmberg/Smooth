//
// Created by permal on 2018-10-27.
//

#ifndef ESP_PLATFORM
#error "This example requires H/W to run on and can only be compiled with IDF."
#endif


#include <chrono>

#include <smooth/core/task_priorities.h>
#include <fstream>
#include <iostream>
#include "spiflash.h"


using namespace smooth;
using namespace smooth::core;
using namespace std::chrono;


namespace spiflash
{
    App::App()
            : Application(APPLICATION_BASE_PRIO,
                          seconds(1))
    {
    }

    void App::init()
    {
        mounted = flash.mount();
        elapsed.start();
    }

    void App::tick()
    {
        if(mounted)
        {
            const char* const file = "/our_root/test.txt";

            {
                std::ofstream out{file, std::ios::binary | std::ios::out};
                if (out.good())
                {
                    out << elapsed.get_running_time().count();
                }
            }

            {
                std::ifstream in{file, std::ios::binary|std::ios::in};
                if(in.good())
                {
                    long count;
                    in >> count;
                    std::cout << "Elapsed time (us): " << count << std::endl;
                }
            }

        }
    }
}
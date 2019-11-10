/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <chrono>

#include "hw_spiflash.h"
#include "smooth/core/task_priorities.h"
#include <fstream>
#include <iostream>
#include "smooth/core/filesystem/MountPoint.h"

using namespace smooth;
using namespace smooth::core;
using namespace std::chrono;
using namespace smooth::core::filesystem;

namespace hw_spiflash
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
        if (mounted)
        {
            const auto file = FlashMount::instance().mount_point() / "test.txt";

            {
                std::ofstream out{ file.str(), std::ios::binary | std::ios::out };

                if (out.good())
                {
                    out << elapsed.get_running_time().count();
                }
            }

            {
                std::ifstream in{ file.str(), std::ios::binary | std::ios::in };

                if (in.good())
                {
                    long count;
                    in >> count;
                    std::cout << "Elapsed time (us): " << count << std::endl;
                }
            }
        }
    }
}

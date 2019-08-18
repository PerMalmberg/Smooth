// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <chrono>

#include "hw_spiflash.h"
#include <smooth/core/task_priorities.h>
#include <fstream>
#include <iostream>
#include <smooth/core/filesystem/MountPoint.h>


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
        if(mounted)
        {
            const auto file = FlashMount::instance().mount_point() / "test.txt";

            {
                std::ofstream out{file.str(), std::ios::binary | std::ios::out};
                if (out.good())
                {
                    out << elapsed.get_running_time().count();
                }
            }

            {
                std::ifstream in{file.str(), std::ios::binary|std::ios::in};
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
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

#include <thread>
#include <vector>
#include <iostream>
#include "catch.hpp"
#include <smooth/core/filesystem/FSLock.h>

using namespace smooth::core::filesystem;

SCENARIO("Single thread")
{
    GIVEN("An initialized FSLock")
    {
        FSLock::set_limit(2);

        WHEN("A thread tries to open a file")
        {
            FSLock lock1{};
            THEN("Open files are one")
            {
                REQUIRE(FSLock::open_files() == 1);
            }
        }
        AND_WHEN("Same thread opens two files")
        {
            THEN("Open files are two")
            {
                {
                    FSLock lock1{};
                    FSLock lock2{};
                    REQUIRE(FSLock::open_files() == 2);
                }

                REQUIRE(FSLock::open_files() == 0);
            }
        }
    }
}

SCENARIO("Two threads opens files")
{
    GIVEN("An initialized FSLock")
    {
        const int max_open = 5;
        FSLock::set_limit(max_open);
        std::vector<std::thread> ts{};

        auto thread_main = []()
                           {
                               FSLock lock1;
                               assert(FSLock::open_files() <= max_open);
                               std::this_thread::sleep_for(std::chrono::microseconds{ 100 });
                           };

        WHEN("Multiple threads opens files")
        {
            THEN("No more than the specified amount of files are open at the same time")
            {
                for (int i = 0; i < 10000; ++i)
                {
                    ts.emplace_back(std::thread{ thread_main });
                }

                for (auto& t : ts)
                {
                    t.join();
                }

                REQUIRE(FSLock::open_files() == 0);
                auto max_ever = FSLock::max_concurrently_opened();
                std::cout << "Max concurrent: " << max_ever << '\n';
                REQUIRE(max_ever <= max_open);
            }
        }
    }
}

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

#include <thread>
#include <vector>
#include <iostream>
#include <catch2/catch.hpp>
#include "smooth/core/filesystem/FSLock.h"

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

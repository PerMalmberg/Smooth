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

#include <limits>
#include <vector>

#include "logging.h"
#include "smooth/core/logging/log.h"

using namespace std;
using namespace smooth::core::logging;
using namespace fmt::literals;

namespace logging
{
    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1))
    {
    }

    void App::init()
    {
    }

    void App::tick()
    {
        std::vector<int> arr{ 1, 2, 3, 4 };

        Log::info("Tag", "Log a 32-bit int: {0:d}"_format (numeric_limits<int32_t>::min()));
        Log::info("Tag", "Log {} {} {:#x} {:#x} {:#x}",
                                "multiple",
                                "hex values of different types:",
                                numeric_limits<uint32_t>::max(),
                                numeric_limits<uint64_t>::max(),
                                numeric_limits<uint8_t>::max());
    }
}

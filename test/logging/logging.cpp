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

#include <limits>
#include <vector>

#include "logging.h"
#include <smooth/core/logging/log.h>

using namespace std;
using namespace smooth::core::logging;

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

        Log::info("Tag", Format("Log a 32-bit int: {1}", Int32(numeric_limits<int32_t>::min())));
        Log::info("Tag", Format("Log a 64-bit int: {1}", Int64(numeric_limits<int64_t>::max())));
        Log::info("Tag", Format("Log {1} {2} {3} {4} {5} vector: {6}",
                                Str("multiple"),
                                Str("hex values of different types:"),
                                Hex<uint32_t>(numeric_limits<uint32_t>::max()),
                                Hex<uint64_t>(numeric_limits<uint64_t>::max()),
                                Hex<uint8_t>(numeric_limits<uint8_t>::max(), true),
                                Vector<int>(arr, false))
                  );
    }
}

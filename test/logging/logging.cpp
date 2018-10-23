//
// Created by permal on 2018-10-21.
//

#include <limits>

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

        Log::info("Tag", Format("Log a 32-bit int: {1}", Int32(numeric_limits<int32_t>::min())));
        Log::info("Tag", Format("Log a 64-bit int: {1}", Int64(numeric_limits<int64_t>::max())));
        Log::info("Tag", Format("Log {1} {2} {3} {4} {5}",
                                Str("multiple"),
                                Str("hex values of different types:"),
                                Hex<uint32_t>(numeric_limits<uint32_t>::max()),
                                Hex<uint64_t>(numeric_limits<uint64_t>::max()),
                                Hex<uint8_t>(numeric_limits<uint8_t>::max(), true))
                                );
    }
}
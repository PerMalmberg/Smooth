#include "smooth/core/logging/log.h"

namespace smooth::core::logging
{
    std::mutex Log::guard{};
    fmt::basic_memory_buffer<char, 500> Log::buff{};
}

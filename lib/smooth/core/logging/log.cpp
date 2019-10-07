#include <smooth/core/logging/log.h>

namespace smooth::core::logging
{
    std::mutex Log::guard{};
    fmt::memory_buffer Log::buff{};
}
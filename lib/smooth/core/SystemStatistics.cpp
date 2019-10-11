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

#include <smooth/core/logging/log.h>
#include <smooth/core/SystemStatistics.h>
#include <fmt/core.h>

#ifdef ESP_PLATFORM

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#endif

using namespace smooth::core::logging;

static constexpr const char* tag = "MemStat";

namespace smooth::core
{
    TaskStats::TaskStats(uint32_t stack_size)
            : stack_size(stack_size)
    {
#ifdef ESP_PLATFORM
        high_water_mark = uxTaskGetStackHighWaterMark(nullptr);
#else
        high_water_mark = 0;
#endif
    }

    void SystemStatistics::dump() const noexcept
    {
#ifdef ESP_PLATFORM
        Log::info(tag, "[INTERNAL]");
        dump_mem_stats(MALLOC_CAP_INTERNAL);
        Log::info(tag, "[INTERNAL | DMA]");
        dump_mem_stats(MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
        Log::info(tag, "[SPIRAM]");
        dump_mem_stats(MALLOC_CAP_SPIRAM);
#endif

        { // Only need to lock while accessing the shared data
            synch guard{ lock };
            constexpr const char* format = "{:>16} | {:>10} | {:>15} | {:>15}";
            Log::info(tag, format, "Name", "Stack", "Min free stack", "Max used stack" );

            for (const auto& stat : task_info)
            {
                Log::info(tag,
                format,
                stat.first,
                stat.second.get_stack_size(),
                stat.second.get_high_water_mark(),
                stat.second.get_stack_size() - stat.second.get_high_water_mark());
            }
        }
    }

#ifdef ESP_PLATFORM

    void SystemStatistics::dump_mem_stats(uint32_t caps) const noexcept
    {
        Log::info(tag, "8-bit F:{:>10} LB:{:>10} M:{:>10} | 32-bit: F:{:>10} LB:{:>10} M:{:>10}",
                              heap_caps_get_free_size(caps | MALLOC_CAP_8BIT),
                              heap_caps_get_largest_free_block(caps | MALLOC_CAP_8BIT),
                              heap_caps_get_minimum_free_size(caps | MALLOC_CAP_8BIT),
                              heap_caps_get_free_size(caps | MALLOC_CAP_32BIT),
                              heap_caps_get_largest_free_block(caps | MALLOC_CAP_32BIT),
                              heap_caps_get_minimum_free_size(caps | MALLOC_CAP_32BIT));
    }

#endif
}

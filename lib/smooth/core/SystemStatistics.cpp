// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2019 Per Malmberg (https://github.com/PerMalmberg)
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

#include <smooth/core/logging/log.h>
#include <smooth/core/SystemStatistics.h>

#ifdef ESP_PLATFORM

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#endif

using namespace smooth::core::logging;

static const char* tag = "MemStat";

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
        //auto current_free = esp_get_free_heap_size();
        //auto all_time_free_low = esp_get_minimum_free_heap_size();

        Log::info(tag, "[INTERNAL]");
        dump_mem_stats(MALLOC_CAP_INTERNAL);
        Log::info(tag, "[SPIRAM]");
        dump_mem_stats(MALLOC_CAP_SPIRAM);

        { // Only need to lock while accessing the shared data
            synch guard{lock};
            Log::info(tag, Format("Name\tStack\tMin free stack"));
            for (const auto& stat : task_info)
            {
                Log::info(tag, Format("{1}\t{2}\t{3}", Str(stat.first), UInt32(stat.second.get_stack_size()),
                                      UInt32(stat.second.get_high_water_mark())));
            }
        }
    }

    void SystemStatistics::dump_mem_stats(uint32_t caps) const noexcept
    {
        Log::info(tag, Format("8-bit F:{1} LB:{2} M:{3} | 32-bit: F:{4} LB:{5} M:{6}",
                              UInt32(heap_caps_get_free_size(caps | MALLOC_CAP_8BIT)),
                              UInt32(heap_caps_get_largest_free_block(caps | MALLOC_CAP_8BIT)),
                              UInt32(heap_caps_get_minimum_free_size(caps | MALLOC_CAP_8BIT)),
                              UInt32(heap_caps_get_free_size(caps | MALLOC_CAP_32BIT)),
                              UInt32(heap_caps_get_largest_free_block(caps | MALLOC_CAP_32BIT)),
                              UInt32(heap_caps_get_minimum_free_size(caps | MALLOC_CAP_32BIT))));
    }
}
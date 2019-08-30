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

#include <unordered_map>
#include <mutex>

namespace smooth::core
{
    class TaskStats
    {
        public:
            TaskStats() = default;

            explicit TaskStats(uint32_t stack_size);

            TaskStats(const TaskStats&) = default;

            TaskStats(TaskStats&&) = default;

            TaskStats& operator=(const TaskStats&) = default;

            TaskStats& operator=(TaskStats&&) = default;

            [[nodiscard]] uint32_t get_stack_size() const noexcept
            {
                return stack_size;
            }

            [[nodiscard]] uint32_t get_high_water_mark() const noexcept
            {
                return high_water_mark;
            }

        private:
            uint32_t stack_size{};
            uint32_t high_water_mark{};
    };

    class SystemStatistics
    {
        public:
            using synch = std::lock_guard<std::mutex>;

            static SystemStatistics& instance()
            {
                static SystemStatistics instance{};

                return instance;
            }

            void report(const std::string& task_name, TaskStats&& stats) noexcept
            {
                synch guard{ lock };
                task_info[task_name] = stats;
            }

            void dump() const noexcept;

        private:
#ifdef ESP_PLATFORM

            void dump_mem_stats(uint32_t caps) const noexcept;

#endif

            mutable std::mutex lock{};
            std::unordered_map<std::string, TaskStats> task_info{};
    };
}

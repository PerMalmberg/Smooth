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

    /// \brief Displays system statistics; memory and stack usage.
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

            void dump_mem_stats(const char* header, uint32_t caps) const noexcept;

#endif

            mutable std::mutex lock{};
            std::unordered_map<std::string, TaskStats> task_info{};
    };
}

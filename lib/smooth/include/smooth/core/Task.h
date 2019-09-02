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

#pragma once

#include <string>
#include <chrono>
#include <cstdint>
#include <map>
#include <mutex>
#include <condition_variable>

#include <thread>

#include <smooth/core/ipc/ITaskEventQueue.h>
#include <smooth/core/ipc/IPolledTaskQueue.h>
#include <smooth/core/ipc/QueueNotification.h>
#include <smooth/core/ipc/Queue.h>
#include <smooth/core/timer/ElapsedTime.h>
#include <atomic>

#ifdef ESP_PLATFORM
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <sdkconfig.h>

#if CONFIG_FREERTOS_HZ < 1000
#error "Smooth requires CONFIG_FREERTOS_HZ at 1000Hz for proper functionality (spec. timers)"
#endif
#else
const int tskNO_AFFINITY = std::numeric_limits<int>::max();
#endif

namespace smooth::core
{
    /// The Task class encapsulates management and execution of a task.
    /// The intent is to provide the scaffolding needed by nearly every task in an
    /// embedded system; an initialization method, a periodically called tick(),
    /// the ability to receive events in a thread-safe manner.
    class Task
    {
        public:
            virtual ~Task();

            /// Starts the task.
            void start();

            void register_queue_with_task(smooth::core::ipc::ITaskEventQueue* task_queue);

            void register_polled_queue_with_task(smooth::core::ipc::IPolledTaskQueue* polled_queue);

            void unregister_polled_queue_with_task(smooth::core::ipc::IPolledTaskQueue* polled_queue);

            Task(const Task&) = delete;

            Task& operator=(const Task&) = delete;

            Task(Task&&) = delete;

            Task& operator=(Task&&) = delete;

        protected:
            /// Use this constructor to attach to an existing task, i.e. the main task.
            /// \param task_to_attach_to The task to attach to.
            /// \param priority Task priority
            /// \param tick_interval Tick interval
            Task(uint32_t priority, std::chrono::milliseconds tick_interval);

            /// Use this constructor when creating your own task.
            /// \param task_name Name of task.
            /// \param stack_size Tack size, in bytes.
            /// \param priority Task priority
            /// \param tick_interval Tick interval
            /// \param core Core affinity, defaults to no affinity
            Task(std::string task_name,
                 uint32_t stack_size,
                 uint32_t priority,
                 std::chrono::milliseconds tick_interval,
                 int core = tskNO_AFFINITY);

            /// The tick() method is where the task shall perform its work.
            /// It is called every 'tick_interval' when there no events available.
            /// Note that if there is a constant stream of event received via a TaskEventQueue,
            /// the tick may be delayed (depending on the tick_interval).
            virtual void tick()
            {
            }

            /// Called once when task is started.
            virtual void init()
            {
            }

            void report_stack_status();

            const std::string name;
        private:
            void exec();

            std::thread worker;
            uint32_t stack_size;
            uint32_t priority;
            std::chrono::milliseconds tick_interval;
            smooth::core::ipc::QueueNotification notification{};
            bool is_attached;
            int affinity;
            std::atomic_bool started{ false };
            std::mutex start_mutex{};
            std::mutex queue_mutex{};
            std::condition_variable start_condition{};
            smooth::core::timer::ElapsedTime status_report_timer{};
            std::vector<smooth::core::ipc::IPolledTaskQueue*> polled_queues{};
    };
}

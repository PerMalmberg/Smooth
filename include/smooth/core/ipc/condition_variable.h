#pragma once

#include <mutex>
#include <chrono>
#include <thread>
#include <vector>

#ifdef ESP_PLATFORM
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#else

#include <condition_variable>

#endif

namespace smooth
{
    namespace core
    {
        class Task;

        namespace ipc
        {
            /// This is a poor man's replacement for std::condition variable to be used until
            /// a full implementation is provided with the xtensa-gcc toolchain.
            ///
            /// Limitations:
            /// * Only supports a single waiting thread.
            /// * Requires the owning smooth::core::Task& as a constructor parameter
            class condition_variable
            {
                public:
                    explicit condition_variable(smooth::core::Task& parent);

                    bool
                    wait_for(std::unique_lock<std::mutex>& lock,
                             const std::chrono::steady_clock::duration& duration,
                             std::function<bool()> pred = nullptr);

                    bool wait_until(std::unique_lock<std::mutex>& lock,
                                    const std::chrono::steady_clock::time_point& end_time,
                                    std::function<bool()> pred = nullptr);

                    void notify_one();
                private:
                    Task& parent;
#ifndef ESP_PLATFORM
                    std::condition_variable cond{};
#endif
            };
        }
    }

}

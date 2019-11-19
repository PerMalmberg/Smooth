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

#pragma once

#include <string>
#include <chrono>
#include <functional>
#include "smooth/core/timer/Timer.h"
#include "smooth/core/timer/TimerExpiredEvent.h"
#include "smooth/core/ipc/TaskEventQueue.h"

namespace smooth::core::timer
{
    class TimerService;

    class Timer;

    /// RAII helper for Timer.
    class TimerOwner
    {
        public:
            TimerOwner(int id,
                       const std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>>& event_queue,
                       bool auto_reload,
                       std::chrono::milliseconds interval);

            TimerOwner() = default;

            TimerOwner(const TimerOwner&) = default;

            TimerOwner& operator=(const TimerOwner&) = default;

            TimerOwner(TimerOwner&&) = default;

            TimerOwner& operator=(TimerOwner&&) = default;

            ~TimerOwner();

            std::shared_ptr<Timer> operator->() const noexcept;

            explicit operator bool() const noexcept
            {
                return static_cast<bool>(t);
            }
        private:
            friend Timer;
            explicit TimerOwner(std::shared_ptr<Timer> t) noexcept;

            std::shared_ptr<Timer> t;
    };

    /// A timer ensures that a context switch is made to the correct task before any processing takes place.
    /// This is done by sending an event on the provided event queue.
    class Timer
        : public ITimer, public std::enable_shared_from_this<Timer>
    {
        public:
            /// Factory method
            /// \param id The ID of the timer. Solely for use by the application programmer.
            /// \param event_queue The vent queue to send events on.
            /// \param auto_reload If true, the timer will restart itself when it expires.
            /// \param interval The interval between the start time and when the timer expiers.
            static TimerOwner create(int id,
                                     const std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>>& event_queue,
                                     bool auto_reload,
                                     std::chrono::milliseconds interval);

            // When the destructor runs for a Timer, it means the TimerService cannot be holding any shared_ptr<>
            // to the current instance (if it did, the destructor wouldn't be running)
            // Thus, we do not need to do anything to remove the instance from the TimerService. In fact, we MUST NOT
            // call  shared_from_this() from within the destructor as that will result in a std::bad_weak_ptr exception
            // being thrown.

            // If you're looking at this text trying to figure out why your timers are still running even though
            // you no longer have any references to them (i.e. your shared_ptr<Timer> have been reset() or re-assinged)
            // you should be aware that timers that are recurring always are held via a shared_ptr<> by the TimerService
            // until they are stopped.
            // Likewise, non-recurring timers are held by the TimerService until they expire or stopped.
            // Use a TimerOwner for RAII-style destruction.
            ~Timer() override = default;

            /// Starts the timer
            void start() override;

            /// Starts the timer with the given interval.
            void start(std::chrono::milliseconds interval) override;

            // Stops the timer
            void stop() override;

            // Resets the timer (stop and re-start)
            void reset() override;

            /// Gets the i of the timer.
            int get_id() const override;

            /// \returns true if the timer is repeating.
            bool is_repeating() const
            {
                return repeating;
            }

            /// \r Returns the time point where the timer expires.
            std::chrono::steady_clock::time_point expires_at() const;

        protected:
            int id;
            bool repeating;
            std::chrono::milliseconds timer_interval;

            /// Constructor
            /// \param id The ID of the timer. Solely for use by the application programmer.
            /// \param event_queue The vent queue to send events on.
            /// \param auto_reload If true, the timer will restart itself when it expires.
            /// \param interval The interval between the start time and when the timer expiers.
            Timer(int id, std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>> event_queue,
                  bool auto_reload, std::chrono::milliseconds interval);

        private:
            friend class smooth::core::timer::TimerService;

            void expired();

            void calculate_next_execution();

            std::weak_ptr<ipc::TaskEventQueue<TimerExpiredEvent>> queue;
            std::chrono::steady_clock::time_point expire_time;
    };
}

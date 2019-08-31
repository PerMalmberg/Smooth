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
#include <functional>
#include <smooth/core/timer/Timer.h>
#include <smooth/core/timer/TimerExpiredEvent.h>
#include <smooth/core/ipc/TaskEventQueue.h>

namespace smooth::core::timer
{
    class TimerService;

    class Timer;

    class TimerOwner
    {
        public:
            TimerOwner(const std::string& name,
                       int id,
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
            /// \param name The name of the timer, mainly used for debugging and logging.
            /// \param id The ID of the timer. Solely for use by the application programmer.
            /// \param event_queue The vent queue to send events on.
            /// \param auto_reload If true, the timer will restart itself when it expires.
            /// \param interval The interval between the start time and when the timer expiers.
            static TimerOwner create(const std::string& name,
                                     int id,
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

            void start() override;

            void start(std::chrono::milliseconds interval) override;

            void stop() override;

            void reset() override;

            int get_id() const override;

            const std::string& get_name() override;

            bool is_repeating() const
            { return repeating; }

            std::chrono::steady_clock::time_point expires_at() const;

        protected:
            const std::string timer_name;
            int id;
            bool repeating;
            std::chrono::milliseconds timer_interval;

            /// Constructor
            /// \param name The name of the timer, mainly used for debugging and logging.
            /// \param id The ID of the timer. Solely for use by the application programmer.
            /// \param event_queue The vent queue to send events on.
            /// \param auto_reload If true, the timer will restart itself when it expires.
            /// \param interval The interval between the start time and when the timer expiers.
            Timer(std::string name, int id, std::weak_ptr<ipc::TaskEventQueue<timer::TimerExpiredEvent>> event_queue,
                  bool auto_reload, std::chrono::milliseconds interval);

        private:
            friend class smooth::core::timer::TimerService;

            void expired();

            void calculate_next_execution();

            std::weak_ptr<ipc::TaskEventQueue<TimerExpiredEvent>> queue;
            std::chrono::steady_clock::time_point expire_time;
    };
}

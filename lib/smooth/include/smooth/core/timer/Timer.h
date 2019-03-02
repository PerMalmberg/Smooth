//
// Created by permal on 7/13/17.
//

#pragma once

#include <string>
#include <chrono>
#include <functional>
#include <smooth/core/timer/Timer.h>
#include <smooth/core/timer/TimerExpiredEvent.h>
#include <smooth/core/ipc/TaskEventQueue.h>

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            class TimerService;

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
                    static std::shared_ptr<Timer> create(const std::string& name,
                                                  int16_t id,
                                                  ipc::TaskEventQueue<timer::TimerExpiredEvent>& event_queue,
                                                  bool auto_reload,
                                                  std::chrono::milliseconds interval);

                    // When the destructor runs for a Timer, it means the TimerService cannot be holding any shared_ptr<>
                    // to the current instance (if it did, the destructor wouldn't be running)
                    // Thus, we do not need to do anything to remove the instance from the TimerService. In fact, we MUST NOT call
                    // shared_from_this() from within the destructor as that will result in a std::bad_weak_ptr exception being thrown.

                    // If you're looking at this text trying to figure out why your timers are still running even though
                    // you no longer have any references to them (i.e. your shared_ptr<Timer> have been reset() or re-assinged)
                    // you should be aware that timers that are recurring always are held via a shared_ptr<> by the TimerService
                    // until they are stopped.
                    // Likewise, non-recurring timers are held by the TimerService until they expire or stopped.
                    ~Timer() override = default;

                    void start() override;
                    void start(std::chrono::milliseconds interval) override;
                    void stop() override;
                    void reset() override;
                    int16_t get_id() const override;
                    const std::string& get_name() override;
                    bool is_repeating() const { return repeating;}
                    std::chrono::steady_clock::time_point expires_at() const;
                protected:

                    const std::string name;
                    int16_t id;
                    bool repeating;
                    std::chrono::milliseconds interval;
                    /// Constructor
                    /// \param name The name of the timer, mainly used for debugging and logging.
                    /// \param id The ID of the timer. Solely for use by the application programmer.
                    /// \param event_queue The vent queue to send events on.
                    /// \param auto_reload If true, the timer will restart itself when it expires.
                    /// \param interval The interval between the start time and when the timer expiers.
                    Timer(const std::string& name, int16_t id, ipc::TaskEventQueue<timer::TimerExpiredEvent>& event_queue,
                          bool auto_reload, std::chrono::milliseconds interval);

                private:

                    friend class smooth::core::timer::TimerService;
                    
                    void expired();
                    void calculate_next_execution();

                    ipc::TaskEventQueue<TimerExpiredEvent>& event_queue;
                    std::chrono::steady_clock::time_point expire_time;
            };
        }
    }
}
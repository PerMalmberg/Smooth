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
                                                  uint32_t id,
                                                  ipc::TaskEventQueue<timer::TimerExpiredEvent>& event_queue,
                                                  bool auto_reload,
                                                  std::chrono::milliseconds interval);

                    virtual ~Timer();

                    void start() override;
                    void start(std::chrono::milliseconds interval) override;
                    void stop() override;
                    void reset() override;
                    int get_id() const override;
                    const std::string& get_name() override;
                    bool is_repeating() const { return repeating;}
                    std::chrono::high_resolution_clock::time_point expires_at() const;
                protected:

                    const std::string name;
                    uint32_t id;
                    bool repeating;
                    std::chrono::milliseconds interval;
                    /// Constructor
                    /// \param name The name of the timer, mainly used for debugging and logging.
                    /// \param id The ID of the timer. Solely for use by the application programmer.
                    /// \param event_queue The vent queue to send events on.
                    /// \param auto_reload If true, the timer will restart itself when it expires.
                    /// \param interval The interval between the start time and when the timer expiers.
                    Timer(const std::string& name, uint32_t id, ipc::TaskEventQueue<timer::TimerExpiredEvent>& event_queue,
                          bool auto_reload, std::chrono::milliseconds interval);

                private:

                    void destroy();
                    friend class smooth::core::timer::TimerService;
                    void expired();
                    void calculate_next_execution();
                    ipc::TaskEventQueue<TimerExpiredEvent>& event_queue;
                    std::chrono::high_resolution_clock::time_point expire_time;
            };
        }
    }
}
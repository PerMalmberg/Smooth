//
// Created by permal on 7/15/17.
//

#include <smooth/core/timer/Timer.h>
#include <smooth/core/timer/TimerService.h>

using namespace smooth::core::logging;
using namespace std::chrono;

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            Timer::Timer(const std::string& name, uint32_t id, ipc::TaskEventQueue<TimerExpiredEvent>& event_queue,
                         bool repeating, milliseconds interval)
                    : name(name), id(id), repeating(repeating), interval(interval), event_queue(event_queue),
                      expire_time(steady_clock::now())
            {
                // Start the timer service when a timer is used.
                TimerService::start_service();
            }

            Timer::~Timer()
            {
                destroy();
            }

            void Timer::start()
            {
                start(interval);
            }

            void Timer::start(milliseconds interval)
            {
                if (interval != this->interval)
                {
                    this->interval = interval;
                }
                expire_time = steady_clock::now() + interval;
                reset();
            }

            void Timer::stop()
            {
                TimerService::get().remove_timer(shared_from_this());
            }

            void Timer::reset()
            {
                stop();
                TimerService::get().add_timer(shared_from_this());
            }

            int Timer::get_id() const
            {
                return id;
            }

            const std::string& Timer::get_name()
            {
                return name;
            }

            void Timer::destroy()
            {
                stop();
            }

            void Timer::expired()
            {
                TimerExpiredEvent ev(this);
                event_queue.push(ev);
            }

            // This class is only used to allow std::make_shared to create an instance of Timer.
            class ConstructableTimer
                    : public Timer
            {
                public:
                    ConstructableTimer(const std::string& name,
                                       uint32_t id,
                                       ipc::TaskEventQueue<timer::TimerExpiredEvent>& event_queue,
                                       bool auto_reload,
                                       std::chrono::milliseconds interval)
                            : Timer(name, id, event_queue, auto_reload, interval)
                    {
                    }
            };

            std::shared_ptr<Timer> Timer::create(const std::string& name,
                                                 uint32_t id,
                                                 ipc::TaskEventQueue<timer::TimerExpiredEvent>& event_queue,
                                                 bool auto_reload,
                                                 std::chrono::milliseconds interval)
            {
                return std::make_shared<ConstructableTimer>(name, id, event_queue, auto_reload, interval);
            }

            std::chrono::steady_clock::time_point Timer::expires_at() const
            {
                return expire_time;
            }

            void Timer::calculate_next_execution()
            {
                expire_time = steady_clock::now() + interval;
            }
        }
    }
}
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

#include "destructing_event_queues.h"
#include <chrono>
#include <random>
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/timer/Timer.h>
#include <smooth/core/timer/TimerExpiredEvent.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/SystemStatistics.h>

using namespace smooth::core;

namespace destructing_event_queues
{
    using namespace smooth::core::ipc;
    using namespace smooth::core::logging;
    using namespace smooth::core::timer;
    using namespace std::chrono;

    static std::uint64_t expire_count = 0;

    class TimerUser
            : public IEventListener<TimerExpiredEvent>
    {
        public:
            explicit TimerUser(Task& task)
                    : queue(TimerExpiredQueue_t::create("", 10, task, *this)),
                      timer(Timer::create("", 0, queue, true, milliseconds{100}))
            {
                timer->start();
            }

            void event(const TimerExpiredEvent& /*ev*/) override
            {
                expire_count++;
            }

        private:
            using TimerExpiredQueue_t = TaskEventQueue<TimerExpiredEvent>;
            std::shared_ptr<TimerExpiredQueue_t> queue;
            std::shared_ptr<Timer> timer;
    };

    class Worker
            : public smooth::core::Task
    {
        public:
            Worker()
                    : smooth::core::Task("Worker", 9000, APPLICATION_BASE_PRIO+5, std::chrono::milliseconds{500}),
                      rd(), gen(rd()), dis(0, 1)
            {}

            void tick() override
            {
                auto val = dis(gen);
                auto adding = val == 1;

                if (adding)
                {
                    for (int i = 0; i < 50 && users.size() <= 400; ++i)
                    {
                        users.emplace_back(std::make_shared<TimerUser>(*this));
                    }
                }
                else
                {
                    for (int i = 0; i < 50 - val && !users.empty(); ++i)
                    {
                        users.erase(users.begin());
                        removed++;
                    }
                }

                Log::info("Worker",
                          Format("User count: {1}, expire count: {2}",
                                 UInt64(users.size()),
                                 UInt64(expire_count)));
            }

        private:
            std::vector<std::shared_ptr<TimerUser>> users{};
            std::random_device rd;
            std::mt19937 gen;
            std::uniform_int_distribution<> dis;
            std::size_t removed = 0;

    } test_worker;


    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(10))
    {
    }

    void App::init()
    {
        test_worker.start();
    }

    void App::tick()
    {
        SystemStatistics::instance().dump();
    }
}
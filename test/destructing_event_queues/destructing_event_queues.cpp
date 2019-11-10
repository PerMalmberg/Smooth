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

#include "destructing_event_queues.h"
#include <chrono>
#include <random>
#include "smooth/core/Task.h"
#include "smooth/core/task_priorities.h"
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/timer/Timer.h"
#include "smooth/core/timer/TimerExpiredEvent.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/SystemStatistics.h"

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
                    : queue(TimerExpiredQueue_t::create(10, task, *this)),
                      timer(0, queue, true, milliseconds{ 300 })
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
            TimerOwner timer;
    };

    class Worker
        : public smooth::core::Task
    {
        public:
            Worker()
                    : smooth::core::Task("Worker", 9000, APPLICATION_BASE_PRIO + 5, std::chrono::milliseconds{ 500 }),
                      rd(), gen(rd()), dis(0, 1)
            {}

            void tick() override
            {
                auto val = dis(gen);
                auto adding = val == 1;

                if (adding)
                {
                    for (int i = 0; i < 50 && users.size() < 400; ++i)
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

                Log::info("Worker", "User count: {}, expire count: {}", users.size(), expire_count);
            }

        private:
            std::vector<std::shared_ptr<TimerUser>> users{};
            std::random_device rd;
            std::mt19937 gen;
            std::uniform_int_distribution<> dis;
            std::size_t removed = 0;
    };

    Worker test_worker{};

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

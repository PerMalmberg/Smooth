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

#include "destructing_subscribing_event_queues.h"
#include "smooth/core/Task.h"
#include "smooth/core/task_priorities.h"
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/ipc/SubscribingTaskEventQueue.h"
#include "smooth/core/ipc/Publisher.h"
#include "smooth/core/logging/log.h"
#include <random>

using namespace smooth::core;

namespace destructing_subscribing_event_queues
{
    using namespace smooth::core::ipc;
    using namespace smooth::core::logging;

    class Item
    {
    };

    class Sender
        : public smooth::core::Task
    {
        public:
            Sender()
                    : smooth::core::Task("Sender", 9000, APPLICATION_BASE_PRIO, std::chrono::milliseconds{ 10 })
            {}

            void tick() override
            {
                Publisher<Item>::publish(Item{});
            }
    };

    Sender sender{};

    class Receiver
        : public smooth::core::Task,
        public smooth::core::ipc::IEventListener<Item>
    {
        public:
            using ReveiverQueue_t = smooth::core::ipc::SubscribingTaskEventQueue<Item>;

            Receiver()
                    : smooth::core::Task("Receiver", 9000, APPLICATION_BASE_PRIO, std::chrono::milliseconds{ 500 }),
                      rd(), gen(rd()), dis(0, 1)
            {}

            void tick() override
            {
                auto val = dis(gen);
                auto adding = val == 1;

                if (adding)
                {
                    for (int i = 0; i < 10; ++i)
                    {
                        queues.emplace_back(ReveiverQueue_t::create(50, *this, *this));
                    }
                }
                else
                {
                    for (int i = 0; i < 10 - val && !queues.empty(); ++i)
                    {
                        queues.erase(queues.begin());
                        removed++;
                    }
                }
            }

            void event(const Item& /*value*/) override
            {
                static size_t last_count = 0;
                static size_t event_count = 0;
                event_count++;

                if (last_count != queues.size())
                {
                    last_count = queues.size();
                    Log::info("Rec", "Queue count: {}, Removed: {}, Evt count: {}", last_count, removed, event_count);
                }
            }

        private:
            std::vector<std::shared_ptr<ReveiverQueue_t>> queues{};
            std::random_device rd;
            std::mt19937 gen;
            std::uniform_int_distribution<> dis;
            std::size_t removed = 0;
    };

    Receiver receiver{};

    App::App()
            : Application(smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(1))
    {
    }

    void App::init()
    {
        sender.start();
        receiver.start();
    }

    void App::tick()
    {
        std::cout << "Tick!" << std::endl;
    }
}

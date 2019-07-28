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

#include "destructing_subscribing_event_queues.h"
#include <smooth/core/Task.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/ipc/Publisher.h>
#include <smooth/core/logging/log.h>
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
                    : smooth::core::Task("Sender", 9000, APPLICATION_BASE_PRIO, std::chrono::milliseconds{10})
            {}

            void tick() override
            {
                Publisher<Item>::publish(Item{});
            }
    } sender;

    class Receiver
            : public smooth::core::Task,
              public smooth::core::ipc::IEventListener<Item>
    {
        public:
            using ReveiverQueue_t = smooth::core::ipc::SubscribingTaskEventQueue<Item>;

            Receiver()
                    : smooth::core::Task("Receiver", 9000, APPLICATION_BASE_PRIO, std::chrono::milliseconds{500}),
                      rd(), gen(rd()), dis(0, 1)
            {}

            void tick() override
            {
                auto val = dis(gen);
                auto adding = val == 1;

                if (adding)
                {
                    for(int i = 0; i < 10; ++i)
                    {
                        queues.emplace_back(ReveiverQueue_t::create("", 50, *this, *this));
                    }
                }
                else
                {
                    for(int i = 0; i < 10 - val && !queues.empty(); ++i)
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
                if(last_count != queues.size())
                {
                    last_count = queues.size();
                    Log::info("Rec", Format("Queue count: {1}, Removed: {2}, Evt count: {3}", UInt64(last_count), UInt64(removed), UInt64(event_count)));
                }
            }

        private:
            std::vector<std::shared_ptr<ReveiverQueue_t>> queues{};
            std::random_device rd;
            std::mt19937 gen;
            std::uniform_int_distribution<> dis;
            std::size_t removed = 0;

    } receiver;


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
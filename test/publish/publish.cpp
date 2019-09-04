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

#include "publish.h"
#include <smooth/core/task_priorities.h>
#include <smooth/core/json/Value.h>
#include <fstream>
#include <cassert>
#include <smooth/core/logging/log.h>
#include <chrono>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/ipc/Publisher.h>
#include <smooth/core/Application.h>

using namespace smooth;

using namespace smooth::core;
using namespace smooth::core::json;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace publish
{
    PublisherTask::PublisherTask()
            : smooth::core::Task("Publisher", 8192, smooth::core::APPLICATION_BASE_PRIO, milliseconds(1))
    {
    }

    void PublisherTask::init()
    {
    }

    void PublisherTask::tick()
    {
        smooth::core::ipc::Publisher<ItemToPublish>::publish(ItemToPublish(std::chrono::steady_clock::now()));
    }

    App::App()
            : Application(APPLICATION_BASE_PRIO, seconds(1)),
              sub(SubscribeQueue::create("sub", 10, *this, *this))
    {
    }

    void App::init()
    {
        p.start();
    }

    void App::tick()
    {
    }

    void App::event(const ItemToPublish& item)
    {
        // Although this task has tick of one second, this response method will be called approximately
        // every millisecond since that is the pace at which the publisher publishes items.
        // With CONFIG_FREERTOS_HZ at 100, average time is ~150us compared to ~29500us with CONFIG_FREERTOS_HZ
        // at 1000 so there is a noticeable difference in overhead with a higher tick rate.

        auto duration = duration_cast<microseconds>(std::chrono::steady_clock::now() - item.get_start());
        Log::info("event", Format("Time in queue: {1} us", Int64(duration.count())));
    }
}

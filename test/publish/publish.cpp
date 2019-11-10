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

#include "publish.h"
#include "smooth/core/task_priorities.h"
#include "smooth/core/logging/log.h"
#include <chrono>
#include "smooth/core/ipc/SubscribingTaskEventQueue.h"
#include "smooth/core/ipc/Publisher.h"
#include "smooth/core/Application.h"

using namespace smooth;

using namespace smooth::core;
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
              sub(SubscribeQueue::create(10, *this, *this))
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
        Log::info("event", "Time in queue: {} us", duration.count());
    }
}

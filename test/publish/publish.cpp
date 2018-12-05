//
// Created by permal on 2018-10-26.
//

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
              sub("sub", 10, *this, *this)
    {
    }

    void App::init()
    {
        POSIXApplication::init();
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
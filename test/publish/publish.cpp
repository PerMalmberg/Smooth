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
        smooth::core::ipc::Publisher<ItemToPublish>::publish(ItemToPublish(++curr));
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
        elapsed_time.start();
    }

    void App::tick()
    {

    }

    void App::event(const ItemToPublish& item)
    {
        // Although this task has tick of one second, this response method will be called approximately
        // every millisecond since that is the pace at which the publisher publishes items.

        Log::info("event", Format("Got value: {1}, interval us: {2}", Int32(item.val), Int64(elapsed_time.get_running_time().count())));
        elapsed_time.zero();
    }

}
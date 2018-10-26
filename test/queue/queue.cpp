//
// Created by permal on 2018-10-26.
//

#include "queue.h"

#include <smooth/core/logging/log.h>
#include <smooth/core/task_priorities.h>

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace queue
{

    App::App()
            : Application(APPLICATION_BASE_PRIO,
                          std::chrono::milliseconds(1000)),
              queue("string queue", 10, *this, *this),
              sender(queue)
    {
    }

    void App::init()
    {
        Application::init();
        sender.start();
    }

    void App::event(const timer::ElapsedTime& event)
    {
        auto val = event.get_running_time();
        total += val;
        count++;

        if (count % 100 == 0)
        {
            Log::debug("Stats", Format("Avg: {1}us per message, Count: {2}, Last msg: {3}us",
                                       Double(total.count() / (1.0 * count), 2),
                                       Int64(count),
                                       Int64(val.count())));
        }
    }

    SenderTask::SenderTask(TaskEventQueue <timer::ElapsedTime>& out) :
            core::Task("SenderTask", 4096, 10, milliseconds(1)),
            out(out)
    {
    }

    void SenderTask::tick()
    {
        timer::ElapsedTime e;
        e.start();
        out.push(e);
    }
}
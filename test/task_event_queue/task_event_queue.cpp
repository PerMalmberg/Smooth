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

#include "task_event_queue.h"

#include "smooth/core/logging/log.h"
#include "smooth/core/task_priorities.h"

#include <utility>

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace std::chrono;

namespace task_event_queue
{
    App::App()
            : Application(APPLICATION_BASE_PRIO,
                          std::chrono::milliseconds(1000)),
              queue(ElapsedTimeQueue::create(10, *this, *this)),
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
            Log::debug("Stats", "Avg: {}us per message, Count: {}, Last msg: {}us",
                                       static_cast<double>(total.count()) / count, 2,
                                       count,
                                       val.count());
        }
    }

    SenderTask::SenderTask(std::weak_ptr<TaskEventQueue<timer::ElapsedTime>> out)
            :
              core::Task("SenderTask", 4096, 10, milliseconds(1)),
              out(std::move(out))
    {
    }

    void SenderTask::tick()
    {
        timer::ElapsedTime e;
        e.start();
        auto q = out.lock();

        if (q)
        {
            q->push(e);
        }
    }
}

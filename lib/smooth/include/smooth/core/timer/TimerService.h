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

#pragma once

#include <algorithm>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include "smooth/core/Task.h"

namespace smooth::core::timer
{
    class Timer;

    using SharedTimer = std::shared_ptr<Timer>;

    using TimerComparator = std::function<bool (SharedTimer left, SharedTimer right)>;

    /// TimerQueue extends std::priority_queue with the ability to remove an item.
    class TimerQueue
        : public std::priority_queue<SharedTimer, std::vector<SharedTimer>, TimerComparator>
    {
        public:
            explicit TimerQueue(const TimerComparator& cmp)
                    :
                      std::priority_queue<SharedTimer, std::vector<SharedTimer>, TimerComparator>(cmp)
            {
            }

            void remove_timer(const SharedTimer& timer)
            {
                auto it = find_if(c.begin(),
                                  c.end(),
                                  [timer](const SharedTimer& o) {
                                      // Compare pointers
                                      bool found = timer == o;

                                      return found;
                                  });

                if (it != c.end())
                {
                    c.erase(it);
                }
            }
    };

    /// TimerService provides functionality to register a Timer that, when expired results in
    /// a message being posted to the Timer's event queue.
    /// \note You are not meant to use this class directly.
    class TimerService
        : private smooth::core::Task
    {
        public:
            TimerService();

            static void start_service();

            static TimerService& get();

            void add_timer(const SharedTimer& timer);

            void remove_timer(const SharedTimer& timer);

        protected:
            void tick() override;

        private:
            TimerComparator cmp;
            TimerQueue queue;
            std::mutex guard;
            std::condition_variable cond{};
    };
}

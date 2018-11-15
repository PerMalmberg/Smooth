//
// Created by permal on 10/22/17.
//

#pragma once

#include <algorithm>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <smooth/core/Task.h>

namespace smooth
{
    namespace core
    {
        namespace timer
        {
            class Timer;

            typedef std::shared_ptr<Timer> SharedTimer;

            typedef std::function<bool(SharedTimer left,
                                       SharedTimer right)> TimerComparator;

            /// TimerQueue extends std::priority_queue with the ability to remove an item.
            class TimerQueue
                    : public std::priority_queue<SharedTimer, std::vector<SharedTimer>, TimerComparator>
            {
                public:
                    explicit TimerQueue(const TimerComparator& cmp) :
                            std::priority_queue<SharedTimer, std::vector<SharedTimer>, TimerComparator>(cmp)
                    {
                    }

                    void remove_timer(const SharedTimer& timer)
                    {
                        auto it = find_if(c.begin(),
                                          c.end(),
                                          [timer](SharedTimer o)
                                          {
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

                    void add_timer(SharedTimer timer);
                    void remove_timer(SharedTimer timer);
                protected:
                    void tick() override;
                private:
                    TimerComparator cmp;
                    TimerQueue queue;
                    std::mutex guard;
                    std::condition_variable cond{};
                    std::vector<SharedTimer> processed;
            };
        }
    }
}

//
// Created by permal on 2018-10-26.
//

#pragma once

#include <smooth/core/Application.h>
#include <smooth/core/timer/ElapsedTime.h>

namespace publish
{

    class ItemToPublish
    {
        public:
            ItemToPublish() = default;

            explicit ItemToPublish(const std::chrono::steady_clock::time_point& start)
                    : start(start)
            {
            }

            std::chrono::steady_clock::time_point get_start() const
            {
                return start;
            }

        private:
            std::chrono::steady_clock::time_point start;
    };

    class PublisherTask
            : public smooth::core::Task
    {
        public:
            PublisherTask();

            void init() override;

            void tick() override;
    };


    class App
            : public smooth::core::Application,
              smooth::core::ipc::IEventListener<ItemToPublish>
    {
        public:
            App();

            void init() override;

            void tick() override;

            void event(const ItemToPublish&) override;

        private:
            smooth::core::ipc::SubscribingTaskEventQueue<ItemToPublish> sub;
            PublisherTask p{};
    };

}
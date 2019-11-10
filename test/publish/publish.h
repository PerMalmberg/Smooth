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

#include "smooth/core/Application.h"
#include "smooth/core/timer/ElapsedTime.h"

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

            [[nodiscard]] std::chrono::steady_clock::time_point get_start() const
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
            using SubscribeQueue = smooth::core::ipc::SubscribingTaskEventQueue<ItemToPublish>;
            std::shared_ptr<SubscribeQueue> sub;
            PublisherTask p{};
    };
}

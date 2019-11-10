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

#include <iostream>
#include "smooth/core/Application.h"
#include "smooth/core/task_priorities.h"
#include "smooth/core/io/InterruptInput.h"
#include "smooth/core/ipc/ISRTaskEventQueue.h"
#include "smooth/core/ipc/IEventListener.h"

namespace hw_interrupt_queue
{
    class App
        : public smooth::core::Application,
        public smooth::core::ipc::IEventListener<smooth::core::io::InterruptInputEvent>
    {
        public:
            App();

            void init() override;

            void event(const smooth::core::io::InterruptInputEvent& value) override;

        protected:
            using IntrQueue = smooth::core::ipc::ISRTaskEventQueue<smooth::core::io::InterruptInputEvent, 5>;
            std::shared_ptr<IntrQueue> queue;
            smooth::core::io::InterruptInput input;
    };
}

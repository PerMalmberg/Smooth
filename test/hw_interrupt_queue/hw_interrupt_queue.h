// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <iostream>
#include <smooth/core/Application.h>
#include <smooth/core/task_priorities.h>
#include <smooth/core/io/InterruptInput.h>
#include <smooth/core/ipc/ISRTaskEventQueue.h>
#include <smooth/core/ipc/IEventListener.h>

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

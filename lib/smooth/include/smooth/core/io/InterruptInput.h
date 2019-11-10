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

#include "InterruptEvent.h"

#include "Input.h"
#include "smooth/core/ipc/IISRTaskEventQueue.h"
#include <memory>

namespace smooth::core::io
{
    class InterruptInput
        : private Input
    {
        public:
            /// Constructs an InterruptInput
            /// \param queue The queue to put events on
            /// \param io GPIO pin number
            /// \param pull_up Set to true if the input has a pull-up (also enables the internal pull up)
            /// \param pull_down Set to true if the input has a pull-down (also enables the internal pull-down)
            /// \param interrupt_trigger When the interrupt should trigger
            InterruptInput(std::weak_ptr<core::ipc::IISRTaskEventQueue<InterruptInputEvent>> queue, gpio_num_t io,
                           bool pull_up,
                           bool pull_down,
                           gpio_int_type_t interrupt_trigger
                           );

            ~InterruptInput() override;

            void update();

            [[nodiscard]] gpio_num_t get_io() const
            {
                return io;
            }

        private:
            std::weak_ptr<ipc::IISRTaskEventQueue<InterruptInputEvent>> queue;
    };
}

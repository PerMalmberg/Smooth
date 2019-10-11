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

#include "Input.h"

namespace smooth::core::io
{
    class InterruptInputCB
        : private Input
    {
        public:
            using callback = void (void* context);

            /// Constructs an InterruptInputCB
            /// \brief Input with callback running in ISR.
            /// \param cb The callback to attach to the interrupt. Note: called in ISR context.
            /// \param context The context to pass to the callback
            /// \param io GPIO pin number
            /// \param pull_up Set to true if the input has a pull-up (also enables the internal pull up)
            /// \param pull_down Set to true if the input has a pull-down (also enables the internal pull-down)
            /// \param interrupt_trigger When the interrupt should trigger
            InterruptInputCB(callback cb,
                             void* context,
                             gpio_num_t io,
                             bool pull_up,
                             bool pull_down,
                             gpio_int_type_t interrupt_trigger);

            ~InterruptInputCB() override;

            InterruptInputCB() = delete;

            InterruptInputCB(const InterruptInputCB&) = delete;

            InterruptInputCB(const InterruptInputCB&&) = delete;

            InterruptInputCB& operator=(const InterruptInputCB&) = delete;

            InterruptInputCB& operator=(InterruptInputCB&&) = delete;

            [[nodiscard]] gpio_num_t get_io() const
            {
                return io;
            }

        private:
    };
}

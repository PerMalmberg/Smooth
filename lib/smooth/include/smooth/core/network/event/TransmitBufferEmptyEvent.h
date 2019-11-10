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

#include "smooth/core/util/CircularBuffer.h"
#include "smooth/core/network/ISocket.h"

namespace smooth::core::network::event
{
    /// En event sent to the application when all outgoing packets have been sent.
    class TransmitBufferEmptyEvent
    {
        public:
            TransmitBufferEmptyEvent() = default;

            explicit TransmitBufferEmptyEvent(std::shared_ptr<smooth::core::network::ISocket> socket)
                    : s(std::move(socket))
            {
            }

            std::shared_ptr<smooth::core::network::ISocket> get_socket() const
            {
                return s;
            }

        private:
            std::shared_ptr<smooth::core::network::ISocket> s;
    };
}

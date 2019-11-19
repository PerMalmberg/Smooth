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

#include "smooth/core/network/ISocket.h"

namespace smooth::core::network::event
{
    /// Event sent when a socket is connected or disconnected.
    class ConnectionStatusEvent
    {
        public:
            ConnectionStatusEvent() = default;

            ConnectionStatusEvent(const ConnectionStatusEvent&) = default;

            ConnectionStatusEvent(std::shared_ptr<smooth::core::network::ISocket> sock, bool is_connected)
                    : sock(std::move(sock)),
                      connected(is_connected)
            {
            }

            bool is_connected() const
            {
                return connected;
            }

            const std::shared_ptr<ISocket>& get_socket() const
            {
                return sock;
            }

        private:
            std::shared_ptr<smooth::core::network::ISocket> sock;
            bool connected = false;
    };
}

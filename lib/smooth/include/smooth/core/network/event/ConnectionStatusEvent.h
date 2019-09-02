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

#include <smooth/core/network/ISocket.h>

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

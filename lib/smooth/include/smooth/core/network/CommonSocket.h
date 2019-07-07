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

#include "ISocket.h"

#ifndef ESP_PLATFORM
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#endif

#include <chrono>
#include <smooth/core/timer/ElapsedTime.h>

namespace smooth::core::network
{
    class CommonSocket
            : public ISocket,
              public std::enable_shared_from_this<ISocket>
    {
        public:
            CommonSocket()
            {
                // Always start the receive timer so that when no data at all is received, we timeout.
                elapsed_receive_time.start();
            }

            void clear_socket_id() override;

            int get_socket_id() const override;

            void stop() override;

            bool is_active() const override;

            bool restart() override;

            void set_send_timeout(std::chrono::milliseconds timeout) override
            {
                send_timeout = timeout;
            }

            std::chrono::milliseconds get_send_timeout() const override
            {
                return send_timeout;
            }

            void set_receive_timeout(std::chrono::milliseconds timeout) override
            {
                receive_timeout = timeout;
            }

            std::chrono::milliseconds get_receive_timeout() const override
            {
                return receive_timeout;
            }

        protected:
            bool set_non_blocking();

            void log(const char* message);

            void loge(const char* message);

            bool is_connected() const override;

            bool has_send_expired() const override
            {
                return send_timeout.count() > 0
                       && elapsed_send_time.is_running()
                       && elapsed_send_time.get_running_time() > send_timeout;
            }

            bool has_receive_expired() const override
            {
                return receive_timeout.count() > 0
                       && elapsed_receive_time.is_running()
                       && elapsed_receive_time.get_running_time() > receive_timeout;
            }

            std::shared_ptr<InetAddress> ip{};
            bool active = false;
            bool connected = false;
            int socket_id = INVALID_SOCKET;
            std::chrono::milliseconds send_timeout{0};
            std::chrono::milliseconds receive_timeout{0};
            smooth::core::timer::ElapsedTime elapsed_send_time{};
            smooth::core::timer::ElapsedTime elapsed_receive_time{};
    };
}
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

#include "ISocket.h"

#ifndef ESP_PLATFORM
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#endif

#include <chrono>
#include "smooth/core/timer/ElapsedTime.h"

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

            void stop(const char* reason) override;

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
            std::chrono::milliseconds send_timeout{ 0 };
            std::chrono::milliseconds receive_timeout{ 0 };
            smooth::core::timer::ElapsedTime elapsed_send_time{};
            smooth::core::timer::ElapsedTime elapsed_receive_time{};
    };
}

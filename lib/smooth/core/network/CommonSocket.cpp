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

#include "smooth/core/network/CommonSocket.h"
#include <memory>
#include <sys/socket.h>
#include "smooth/core/logging/log.h"
#include "smooth/core/network/SocketDispatcher.h"

using namespace smooth::core::logging;

namespace smooth::core::network
{
    void CommonSocket::clear_socket_id()
    {
        socket_id = INVALID_SOCKET;
    }

    int CommonSocket::get_socket_id() const
    {
        return socket_id;
    }

    bool CommonSocket::is_connected() const
    {
        return connected;
    }

    bool CommonSocket::set_non_blocking()
    {
        bool res = true;

        auto opts = fcntl(socket_id, F_GETFL, 0);

        if (opts < 0)
        {
            loge("Could not get socket flags");
            res = false;
        }
        else if (fcntl(socket_id, F_SETFL, opts | O_NONBLOCK) < 0)
        {
            loge("Could not set non blocking flag");
            res = false;
        }

        return res;
    }

    void CommonSocket::log(const char* message)
    {
        Log::info("Socket",
        "[{}, {}, {}, {}]: {}",
        ip->get_host(),
        ip->get_port(),
        socket_id,
        static_cast<void*>(this),
        message);
    }

    void CommonSocket::loge(const char* message)
    {
        Log::error("Socket", "[{}, {}, {} {}]: {}: {} ({})", ip->get_host(), ip->get_port(),
                   socket_id,
                   static_cast<void*>(this),
                   message,
                   strerror(errno),
                   errno);
    }

    void CommonSocket::stop(const char* reason)
    {
        log(reason);
        stop_internal();
        SocketDispatcher::instance().perform_op(SocketOperation::Op::Stop, shared_from_this());
    }

    bool CommonSocket::is_active() const
    {
        return active;
    }

    bool CommonSocket::restart()
    {
        stop("Restarting");

        return start(ip);
    }
}

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

#include <smooth/core/network/CommonSocket.h>
#include <memory>
#include <sys/socket.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/SocketDispatcher.h>

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
                  Format("[{1}, {2}, {3}, {4}]: {5}",
                         Str(ip->get_host()),
                         Int32(ip->get_port()),
                         Int32(socket_id),
                         Pointer(this),
                         Str(message)));
    }

    void CommonSocket::loge(const char* message)
    {
        Log::error("Socket",
                   Format("[{1}, {2}, {3} {4}]: {5}: {6} ({7})",
                          Str(ip->get_host()),
                          Int32(ip->get_port()),
                          Int32(socket_id),
                          Pointer(this),
                          Str(message),
                          Str(strerror(errno)),
                          Int32(errno)));
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
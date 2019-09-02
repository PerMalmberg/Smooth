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

#include <cstring>
#include <map>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <sys/socket.h>
#include <smooth/core/Task.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include "ISocket.h"
#include "NetworkStatus.h"
#include "SocketOperation.h"
#include "ISocketBackOff.h"

namespace smooth::core::network
{
    /// The SocketDispatcher handles all tasks related to sockets and is responsible for
    /// creating and sending the necessary events to the application. As an application developer
    /// you should never have to care about this class.
    class SocketDispatcher
        : public smooth::core::Task,
        public smooth::core::ipc::IEventListener<NetworkStatus>,
        public smooth::core::ipc::IEventListener<SocketOperation>,
        private ISocketBackOff
    {
        public:
            ~SocketDispatcher() override = default;

            static SocketDispatcher& instance();

            void perform_op(SocketOperation::Op op, std::shared_ptr<ISocket> socket);

            void tick() override;

            void event(const NetworkStatus& event) override;

            void event(const SocketOperation& event) override;

        protected:
        private:
            SocketDispatcher();

            int build_sets();

            void clear_sets();

            void set_timeout();

            void restart_inactive_sockets();

            void remove_socket_from_collection(std::vector<std::shared_ptr<ISocket>>& col,
                                               const std::shared_ptr<ISocket>& socket);

            void remove_socket_from_active_sockets(std::shared_ptr<ISocket>& socket);

            void start_socket(const std::shared_ptr<ISocket>& socket);

            void shutdown_socket(std::shared_ptr<ISocket> socket);

            bool is_backed_off(int socket_id);

            void remove_backed_off_socket(int socket_id);

            void back_off(int socket_id, std::chrono::milliseconds duration) override;

            std::map<int, std::shared_ptr<ISocket>> active_sockets;
            std::vector<std::shared_ptr<ISocket>> inactive_sockets;
            std::mutex socket_guard;
            using NetworkEventQueue = smooth::core::ipc::SubscribingTaskEventQueue<NetworkStatus>;
            std::shared_ptr<NetworkEventQueue> network_events;
            using SocketOperationQueue = smooth::core::ipc::TaskEventQueue<SocketOperation>;
            std::shared_ptr<SocketOperationQueue> socket_op;

            static void set_fd(std::size_t socket_id, fd_set& fd);

            static bool is_fd_set(std::size_t socket_id, fd_set& fd);

            void print_status() const;

            fd_set read_set{};
            fd_set write_set{};
            timeval tv{};
            bool has_ip = false;
            static constexpr const char* tag = "SocketDispatcher";
            std::unordered_map<int, std::chrono::steady_clock::time_point> backed_off{};

            void check_socket_timeouts();
    };
}

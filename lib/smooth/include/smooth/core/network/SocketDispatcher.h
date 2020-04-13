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

#include <cstring>
#include <map>
#include <vector>
#include <mutex>
#include <unordered_map>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <sys/socket.h>
#pragma GCC diagnostic pop
#include "smooth/core/Task.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/ipc/SubscribingTaskEventQueue.h"
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
#ifdef ESP_PLATFORM
            using FD = size_t;
#else
            using FD = int;
#endif
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
                                               const std::shared_ptr<ISocket>& socket) const;

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

            static void set_fd(FD socket_id, fd_set& fd);

            static bool is_fd_set(FD socket_id, fd_set& fd);

            fd_set read_set{};
            fd_set write_set{};
            timeval tv{};
            bool has_ip = false;
            static constexpr const char* tag = "SocketDispatcher";
            std::unordered_map<int, std::chrono::steady_clock::time_point> backed_off{};

            void check_socket_timeouts();
    };
}

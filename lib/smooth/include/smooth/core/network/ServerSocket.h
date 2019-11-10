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

#include <sys/socket.h>
#include <cstring>
#include <memory>
#include "ClientPool.h"
#include "InetAddress.h"
#include "ISocket.h"
#include "IPv4.h"
#include "IPv6.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/network/SocketDispatcher.h"
#include "smooth/core/network/event/ConnectionStatusEvent.h"
#include "smooth/core/network/CommonSocket.h"
#include "smooth/core/network/Socket.h"
#include "smooth/core/util/create_protected.h"

#ifndef ESP_PLATFORM

#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

#endif

namespace smooth::core::network
{
    template<typename Client, typename Protocol, typename ClientContext>
    class ServerSocket
        : public CommonSocket
    {
        public:
            template<typename... ProtocolArguments>
            static std::shared_ptr<ServerSocket<Client, Protocol, ClientContext>>
            create(smooth::core::Task& task, int max_client_count, int backlog, ProtocolArguments... proto_args);

            bool start(std::shared_ptr<InetAddress> bind_to) override;

            void set_client_context(ClientContext* ctx)
            {
                client_context = ctx;
            }

            bool is_server() const override
            {
                return true;
            }

        protected:
            void readable(ISocketBackOff& ops) override;

            void writable() override;

            virtual std::tuple<std::shared_ptr<smooth::core::network::InetAddress>, int>
            accept_request(ISocketBackOff& ops);

            bool has_data_to_transmit() override
            {
                return false;
            }

            bool internal_start() override;

            void publish_connected_status() override
            {
            }

            virtual bool create_socket();

            void stop_internal() override;

            template<typename... ProtocolArguments>
            ServerSocket(smooth::core::Task& task,
                         int max_client_count,
                         int backlog,
                         ProtocolArguments... proto_args)
                    : CommonSocket(),
                      pool(task, max_client_count), backlog(backlog)
            {
                pool.create_clients(proto_args...);
            }

            ClientPool<Client> pool;
            ClientContext* client_context{ nullptr };
        private:
            int backlog{ 0 };
    };

    template<typename Client, typename Protocol, typename ClientContext>
    bool ServerSocket<Client, Protocol, ClientContext>::start(std::shared_ptr<InetAddress> bind_to)
    {
        bool res = false;

        if (!is_active())
        {
            ip = std::move(bind_to);

            // Always do resolve ip to ensure that we are update-to-date.
            res = ip->resolve_ip() && ip->is_valid();

            if (res)
            {
                SocketDispatcher::instance().perform_op(SocketOperation::Op::Start, shared_from_this());
            }
        }

        return res;
    }

    template<typename Client, typename Protocol, typename ClientContext>
    std::tuple<std::shared_ptr<smooth::core::network::InetAddress>, int> ServerSocket<Client, Protocol,
                                                                                      ClientContext>::accept_request(
        ISocketBackOff& ops)
    {
        using namespace smooth::core::logging;

        auto res = std::make_tuple<std::shared_ptr<smooth::core::network::InetAddress>, int>(nullptr, 0);

        if (pool.empty())
        {
            Log::warning("ServerSocket", "No client available at this time");
            ops.back_off(socket_id, DefaultReceiveTimeout);
        }
        else
        {
            sockaddr addr{};
            socklen_t len{ AF_INET6 };

            auto accepted_socket = accept(socket_id, &addr, &len);

            if (accepted_socket == INVALID_SOCKET)
            {
                std::string msg = "Error accepting: ";
                msg += strerror(errno);
                loge(msg.c_str());
            }
            else
            {
                std::shared_ptr<smooth::core::network::InetAddress> ip{};

                if (addr.sa_family == AF_INET)
                {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
                    auto ipv4_address = reinterpret_cast<sockaddr_in*>(&addr);
#pragma GCC diagnostic pop
                    ip = std::make_shared<smooth::core::network::IPv4>(*ipv4_address);
                }
                else
                {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
                    auto ipv6_address = reinterpret_cast<sockaddr_in6*>(&addr);
#pragma GCC diagnostic pop
                    ip = std::make_shared<smooth::core::network::IPv6>(*ipv6_address);
                }

                Log::info("ServerSocket", "Connection accepted");
                res = std::make_tuple<>(ip, accepted_socket);
            }
        }

        return res;
    }

    template<typename Client, typename Protocol, typename ClientContext>
    void ServerSocket<Client, Protocol, ClientContext>::readable(ISocketBackOff& ops)
    {
        const auto& [ip, accepted_socket_id] = accept_request(ops);

        if (ip)
        {
            auto client = pool.get();
            auto socket = Socket<Protocol>::create(ip,
                                                   accepted_socket_id,
                                                   client->get_buffers(),
                                                   client->get_send_timeout());

            client->set_client_context(client_context);
            client->set_socket(socket);
        }
    }

    template<typename Client, typename Protocol, typename ClientContext>
    void ServerSocket<Client, Protocol, ClientContext>::writable()
    {
    }

    template<typename Client, typename Protocol, typename ClientContext>
    void ServerSocket<Client, Protocol, ClientContext>::stop_internal()
    {
        if (is_active())
        {
            log("Server stopping");
            active = false;
            connected = false;
        }
    }

    template<typename Client, typename Protocol, typename ClientContext>
    bool ServerSocket<Client, Protocol, ClientContext>::internal_start()
    {
        bool res = false;

        if (!is_active())
        {
            if (create_socket())
            {
                int reuseaddr = 1;
                setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));

                auto bind_res = bind(socket_id, ip->get_socket_address(), ip->get_socket_address_length());

                if (bind_res == 0)
                {
                    auto listen_res = listen(socket_id, backlog);

                    if (listen_res == 0)
                    {
                        connected = true;
                        active = true;
                        res = true;
                    }
                    else
                    {
                        std::string msg = "Error listening: ";
                        msg += strerror(errno);
                        loge(msg.c_str());
                    }
                }
                else
                {
                    std::string msg = "Error binding: ";
                    msg += strerror(errno);
                    loge(msg.c_str());
                }
            }

            if (!res)
            {
                stop("ServerSocket failed to start");
            }
        }

        return res;
    }

    template<typename Client, typename Protocol, typename ClientContext>
    template<typename... ProtocolArguments>
    std::shared_ptr<ServerSocket<Client, Protocol, ClientContext>> ServerSocket<Client, Protocol,
                                                                                ClientContext>::create(
        smooth::core::Task& task, int max_client_count, int backlog, ProtocolArguments... proto_args)
    {
        return smooth::core::util::create_protected_shared<ServerSocket<Client, Protocol, ClientContext>>(task,
                                                                                                          max_client_count,
                                                                                                          backlog,
                                                                                                          proto_args...);
    }

    template<typename Client, typename Protocol, typename ClientContext>
    bool ServerSocket<Client, Protocol, ClientContext>::create_socket()
    {
        bool res = false;

        if (socket_id < 0)
        {
            socket_id = socket(ip->get_protocol_family(), SOCK_STREAM, 0);

            if (socket_id == INVALID_SOCKET)
            {
                loge("Failed to create server socket");
            }
            else
            {
                res = set_non_blocking();
                int no_delay = 1;
                res &= setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(no_delay)) == 0;

                if (res)
                {
                    log("Created server socket");
                }
                else
                {
                    loge("Failed to set server socket options");
                }
            }
        }
        else
        {
            res = true;
        }

        return res;
    }
}

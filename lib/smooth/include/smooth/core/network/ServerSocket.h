#pragma once

#include <sys/socket.h>
#include <cstring>
#include <memory>
#include "ClientPool.h"
#include "InetAddress.h"
#include "ISocket.h"
#include "IPv4.h"
#include "IPv6.h"
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/SocketDispatcher.h>
#include <smooth/core/network/event/ConnectionStatusEvent.h>
#include <smooth/core/network/CommonSocket.h>
#include <smooth/core/network/Socket.h>

#ifndef ESP_PLATFORM

#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

#endif

using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        namespace network
        {
            template<typename Client, typename Protocol>
            class ServerSocket
                    : public CommonSocket
            {
                public:
                    static std::shared_ptr<ServerSocket<Client,Protocol>>
                    create(smooth::core::Task& task, int max_client_count);

                    bool start(std::shared_ptr<InetAddress> ip) override;

                protected:
                    bool has_send_expired() const override
                    {
                        return false;
                    }

                    void readable() override;

                    virtual std::tuple<std::shared_ptr<smooth::core::network::InetAddress>, int> accept_request();

                    void writable() override;

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

                    void clear_socket_id() override;


                    ServerSocket(smooth::core::Task& task, int max_client_count)
                            : pool(task, max_client_count)
                    {
                    }

                    ClientPool<Client> pool;
            };

            template<typename Client, typename Protocol>
            bool ServerSocket<Client, Protocol>::start(std::shared_ptr<InetAddress> ip)
            {
                bool res = false;
                if (!is_active())
                {
                    this->ip = ip;

                    // Always do resolve ip to ensure that we are update-to-date.
                    res = ip->resolve_ip() && ip->is_valid();

                    if (res)
                    {
                        SocketDispatcher::instance().perform_op(SocketOperation::Op::Start, shared_from_this());
                    }
                }

                return res;
            }

            template<typename Client, typename Protocol>
            std::tuple<std::shared_ptr<smooth::core::network::InetAddress>, int>
            ServerSocket<Client, Protocol>::accept_request()
            {
                auto res = std::make_tuple<std::shared_ptr<smooth::core::network::InetAddress>, int>(nullptr, 0);

                sockaddr addr{};
                socklen_t len{AF_INET6};

                auto accepted_socket = accept(socket_id, &addr, &len);
                if (accepted_socket == INVALID_SOCKET)
                {
                    std::string msg = "Error accepting: ";
                    msg += strerror(errno);
                    loge(msg.c_str());
                }
                else
                {
                    if (pool.empty())
                    {
                        Log::warning("ServerSocket", "No available client, rejecting socket.");
                        shutdown(accepted_socket, SHUT_RDWR);
                        close(accepted_socket);
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

                        res = std::make_tuple<>(ip, accepted_socket);
                    }
                }

                return res;
            }

            template<typename Client, typename Protocol>
            void ServerSocket<Client, Protocol>::readable()
            {
                auto accepted = accept_request();
                auto ip = std::get<0>(accepted);
                auto socket_id = std::get<1>(accepted);

                if (ip)
                {
                    auto client = pool.get();
                    auto socket = Socket<Protocol>::create(ip,
                                                           socket_id,
                                                           client->get_buffers(),
                                                           client->get_send_timeout());

                    client->set_socket(socket);
                }

            }

            template<typename Client, typename Protocol>
            void ServerSocket<Client, Protocol>::writable()
            {
            }

            template<typename Client, typename Protocol>
            void ServerSocket<Client, Protocol>::stop_internal()
            {
                if (is_active())
                {
                    log("Server stopping");
                    active = false;
                    connected = false;
                }
            }

            template<typename Client, typename Protocol>
            void ServerSocket<Client, Protocol>::clear_socket_id()
            {
                socket_id = INVALID_SOCKET;
            }

            template<typename Client, typename Protocol>
            bool ServerSocket<Client, Protocol>::internal_start()
            {
                bool res = false;

                if (!is_active())
                {
                    if (create_socket())
                    {
                        auto bind_res = bind(socket_id, ip->get_socket_address(), ip->get_socket_address_length());
                        if (bind_res == 0)
                        {
                            auto listen_res = listen(socket_id, ip->get_port());
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
                        stop();
                    }
                }

                return res;
            }

            template<typename Client, typename Protocol>
            std::shared_ptr<ServerSocket<Client, Protocol>> ServerSocket<Client, Protocol>::create(
                    smooth::core::Task& task, int max_client_count)
            {
                // This class is solely used to enabled access to the protected ServerSocket constructor from std::make_shared<>
                class MakeSharedActivator
                        : public ServerSocket<Client, Protocol>
                {
                    public:
                        explicit MakeSharedActivator(smooth::core::Task& task, int max_client_count)
                                : ServerSocket<Client, Protocol>(task, max_client_count)
                        {
                        }

                };

                return std::make_shared<MakeSharedActivator>(task, max_client_count);
            }

            template<typename Client, typename Protocol>
            bool ServerSocket<Client, Protocol>::create_socket()
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
    }
}
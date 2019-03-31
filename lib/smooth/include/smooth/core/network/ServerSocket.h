#pragma once

#include <sys/socket.h>
#include <cstring>
#include <memory>
#include "InetAddress.h"
#include "ISocket.h"
#include "IPv4.h"
#include "IPv6.h"
#include "IServerSocketTaskFactory.h"
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/SocketDispatcher.h>
#include <smooth/core/network/event/ConnectionStatusEvent.h>
#include <smooth/core/network/event/ClientConnectedEvent.h>
#include <smooth/core/network/CommonSocket.h>

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
            template<typename Protocol, typename ProtocolClient>
            class ServerSocket
                    : public CommonSocket
            {
                public:
                    static std::shared_ptr<ISocket>
                    create(smooth::core::ipc::TaskEventQueue<event::ClientConnectedEvent<ProtocolClient>>& client_connected,
                           IServerSocketTaskFactory& task_factory);

                    bool start(std::shared_ptr<InetAddress> ip) override;

                    bool has_send_expired() const override
                    {
                        return false;
                    }

                protected:
                    void readable() override;

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


                    ServerSocket(
                            smooth::core::ipc::TaskEventQueue<event::ClientConnectedEvent<ProtocolClient>>& client_connected,
                            IServerSocketTaskFactory& task_factory)
                            : connected_receiver(client_connected),
                              task_provider(task_factory)
                    {
                    }

                private:
                    smooth::core::ipc::TaskEventQueue<event::ClientConnectedEvent<ProtocolClient>>& connected_receiver;
                    IServerSocketTaskFactory& task_provider;
            };

            template<typename Protocol, typename ProtocolClient>
            bool ServerSocket<Protocol, ProtocolClient>::start(std::shared_ptr<InetAddress> ip)
            {
                bool res = false;
                if (!started)
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

            template<typename Protocol, typename ProtocolClient>
            void ServerSocket<Protocol, ProtocolClient>::readable()
            {
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
                    std::shared_ptr<smooth::core::network::InetAddress> ip{};
                    if (addr.sa_family == AF_INET)
                    {
                        auto ipv4_address = reinterpret_cast<sockaddr_in*>(&addr);
                        ip = std::make_shared<smooth::core::network::IPv4>(*ipv4_address);
                    }
                    else
                    {
                        auto ipv6_address = reinterpret_cast<sockaddr_in6*>(&addr);
                        ip = std::make_shared<smooth::core::network::IPv6>(*ipv6_address);
                    }

                    auto client = std::make_shared<ProtocolClient>(task_provider.get_task());
                    auto socket = Socket<Protocol>::create(ip, accepted_socket, client->get_buffers(), client->get_send_timeout());
                    client->set_socket(socket);

                    connected_receiver.push(event::ClientConnectedEvent<ProtocolClient>(client));
                }
            }

            template<typename Protocol, typename ProtocolClient>
            void ServerSocket<Protocol, ProtocolClient>::writable()
            {
            }

            template<typename Protocol, typename ProtocolClient>
            void ServerSocket<Protocol, ProtocolClient>::stop_internal()
            {
                if (started)
                {
                    log("Server stopping");
                    started = false;
                    connected = false;
                }
            }

            template<typename Protocol, typename ProtocolClient>
            void ServerSocket<Protocol, ProtocolClient>::clear_socket_id()
            {
                socket_id = INVALID_SOCKET;
            }

            template<typename Protocol, typename ProtocolClient>
            bool ServerSocket<Protocol, ProtocolClient>::internal_start()
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
                                started = true;
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

            template<typename Protocol, typename ProtocolClient>
            std::shared_ptr<ISocket> ServerSocket<Protocol, ProtocolClient>::create(
                    smooth::core::ipc::TaskEventQueue<event::ClientConnectedEvent<ProtocolClient>>& client_connected,
                    IServerSocketTaskFactory& task_factory)
            {
                // This class is solely used to enabled access to the protected ServerSocket<Protocol> constructor from std::make_shared<>
                class MakeSharedActivator
                        : public ServerSocket<Protocol, ProtocolClient>
                {
                    public:
                        explicit MakeSharedActivator(
                                smooth::core::ipc::TaskEventQueue<event::ClientConnectedEvent<ProtocolClient>>& client_connected,
                                IServerSocketTaskFactory& task_factory)
                                : ServerSocket<Protocol, ProtocolClient>(client_connected,
                                                                         task_factory)
                        {
                        }

                };

                return std::make_shared<MakeSharedActivator>(client_connected, task_factory);
            }

            template<typename Protocol, typename ProtocolClient>
            bool ServerSocket<Protocol, ProtocolClient>::create_socket()
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
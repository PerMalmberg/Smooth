#pragma once

#include <sys/socket.h>
#include <cstring>
#include <memory>
#include "InetAddress.h"
#include "ISocket.h"
#include "IPv4.h"
#include "IPv6.h"
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/SocketDispatcher.h>
#include <smooth/core/network/ConnectionStatusEvent.h>
#include <smooth/core/network/ClientConnectedEvent.h>
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
            template<typename Protocol>
            class ServerSocket
                    : public CommonSocket
            {
                public:
                    static std::shared_ptr<ISocket> create(
                            smooth::core::ipc::TaskEventQueue<smooth::core::network::ClientConnectedEvent<Protocol>>& client_connected,
                            IPacketSendBuffer<Protocol>& client_tx_buffer,
                            IPacketReceiveBuffer<Protocol>& client_rx_buffer,
                            smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& client_tx_empty,
                            smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Protocol>>& client_data_available,
                            smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& client_connection_status,
                            std::chrono::milliseconds client_send_timeout);

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


                    ServerSocket(smooth::core::ipc::TaskEventQueue<ClientConnectedEvent<Protocol>>& client_connected,
                                 IPacketSendBuffer<Protocol>& client_tx_buffer,
                                 IPacketReceiveBuffer<Protocol>& client_rx_buffer,
                                 smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& client_tx_empty,
                                 smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Protocol>>& client_data_available,
                                 smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& client_connection_status,
                                 std::chrono::milliseconds client_send_timeout)
                            : connected_receiver(client_connected),
                              tx_buffer(client_tx_buffer),
                              rx_buffer(client_rx_buffer),
                              tx_empty(client_tx_empty),
                              data_available(client_data_available),
                              connection_status(client_connection_status),
                              send_timeout(client_send_timeout)
                    {
                    }

                private:
                    smooth::core::ipc::TaskEventQueue<ClientConnectedEvent<Protocol>>& connected_receiver;
                    IPacketSendBuffer<Protocol>& tx_buffer;
                    IPacketReceiveBuffer<Protocol>& rx_buffer;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Protocol>>& data_available;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status;
                    std::chrono::milliseconds send_timeout;
            };

            template<typename Protocol>
            bool ServerSocket<Protocol>::start(std::shared_ptr<InetAddress> ip)
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

            template<typename Protocol>
            void ServerSocket<Protocol>::readable()
            {
                sockaddr addr{};
                socklen_t len{AF_INET6};

                auto accept_id = accept(socket_id, &addr, &len);
                if (accept_id == INVALID_SOCKET)
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

                    auto client = Socket<Protocol>::create(ip, accept_id, tx_buffer, rx_buffer, tx_empty, data_available, connection_status, send_timeout);
                    connected_receiver.push(ClientConnectedEvent<Protocol>(client));
                }
            }

            template<typename Protocol>
            void ServerSocket<Protocol>::writable()
            {

            }

            template<typename Protocol>
            void ServerSocket<Protocol>::stop_internal()
            {
                if (started)
                {
                    log("Server stopping");
                    started = false;
                    connected = false;
                }
            }

            template<typename Protocol>
            void ServerSocket<Protocol>::clear_socket_id()
            {
                socket_id = INVALID_SOCKET;
            }

            template<typename Protocol>
            bool ServerSocket<Protocol>::internal_start()
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

            template<typename Protocol>
            std::shared_ptr<ISocket> ServerSocket<Protocol>::create(
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::ClientConnectedEvent<Protocol>>& client_connected,
                    IPacketSendBuffer<Protocol>& client_tx_buffer,
                    IPacketReceiveBuffer<Protocol>& client_rx_buffer,
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& client_tx_empty,
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Protocol>>& client_data_available,
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& client_connection_status,
                    std::chrono::milliseconds client_send_timeout)
            {
                // This class is solely used to enabled access to the protected ServerSocket<Protocol> constructor from std::make_shared<>
                class MakeSharedActivator
                        : public ServerSocket<Protocol>
                {
                    public:
                        explicit MakeSharedActivator(
                                smooth::core::ipc::TaskEventQueue<smooth::core::network::ClientConnectedEvent<Protocol>>& client_connected,
                                IPacketSendBuffer<Protocol>& client_tx_buffer,
                                IPacketReceiveBuffer<Protocol>& client_rx_buffer,
                                smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& client_tx_empty,
                                smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Protocol>>& client_data_available,
                                smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& client_connection_status,
                                std::chrono::milliseconds client_send_timeout)
                                : ServerSocket<Protocol>(client_connected,
                                                         client_tx_buffer,
                                                         client_rx_buffer,
                                                         client_tx_empty,
                                                         client_data_available,
                                                         client_connection_status,
                                                         client_send_timeout)
                        {
                        }

                };

                return std::make_shared<MakeSharedActivator>(client_connected,
                                                             client_tx_buffer,
                                                             client_rx_buffer,
                                                             client_tx_empty,
                                                             client_data_available,
                                                             client_connection_status,
                                                             client_send_timeout);
            }

            template<typename Protocol>
            bool ServerSocket<Protocol>::create_socket()
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
//
// Created by permal on 6/25/17.
//

#pragma once

#include <cstring>
#include <sys/socket.h>

#undef bind

#include "InetAddress.h"
#include "ISocket.h"
#include <array>
#include <memory>
#include <openssl/ssl.h>
#include <smooth/core/util/CircularBuffer.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/network/TransmitBufferEmptyEvent.h>
#include <smooth/core/network/DataAvailableEvent.h>
#include <smooth/core/network/PacketSendBuffer.h>
#include <smooth/core/network/SocketDispatcher.h>
#include <smooth/core/network/ConnectionStatusEvent.h>

namespace smooth
{
    namespace core
    {
        namespace network
        {

            template<typename T>
            class Socket
                    : public ISocket, public std::enable_shared_from_this<ISocket>
            {
                public:
                    friend class smooth::core::network::SocketDispatcher;

                    static std::shared_ptr<ISocket>
                    create(IPacketSendBuffer<T>& tx_buffer, IPacketReceiveBuffer<T>& rx_buffer,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<T>>& data_available,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status);

                    virtual ~Socket()
                    {
                        log("Destructing");
                    }

                    bool start(std::shared_ptr<InetAddress> ip);
                    void stop() override;
                    bool restart() override;
                    bool is_active() override;

                    void readable() override;

                    void writable() override;

                protected:
                    Socket(IPacketSendBuffer<T>& tx_buffer, IPacketReceiveBuffer<T>& rx_buffer,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<T>>& data_available,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status);

                    virtual bool create_socket();

                    virtual bool prepare_connected_socket()
                    {
                        return true;
                    }

                    int get_socket_id() override
                    {
                        return socket_id;
                    }

                    virtual void read_data(uint8_t* target, int max_length);
                    virtual void write_data(const uint8_t* src, int length);

                    IPacketSendBuffer<T>& tx_buffer;
                    IPacketReceiveBuffer<T>& rx_buffer;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<T>>& data_available;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty;

                private:

                    void internal_start() override;

                    bool is_started() override;

                    bool is_connected() override
                    {
                        return connected;
                    }

                    bool set_non_blocking();

                    bool has_data_to_transmit() override
                    {
                        // Also check on connected state so that we don't try to send data
                        // when the socket just has been closed while in SocketDispatcher::tick()
                        return connected && !tx_buffer.is_empty();
                    }

                    void log(const char* message);
                    void loge(const char* message);

                    void publish_connected_status(std::shared_ptr<ISocket>& socket) override;
                    bool check_if_connection_is_completed() override;
                    int socket_id = -1;
                    std::shared_ptr<InetAddress> ip;
                    bool started = false;
                    bool connected = false;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status;
            };


            template<typename T>
            std::shared_ptr<ISocket> Socket<T>::create(IPacketSendBuffer<T>& tx_buffer,
                                                       IPacketReceiveBuffer<T>& rx_buffer,
                                                       smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                                       smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<T>>& data_available,
                                                       smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status)
            {

                // This class is solely used to enabled access to the protected Socket<T> constructor from std::make_shared<>
                class MakeSharedActivator
                        : public Socket<T>
                {
                    public:
                        MakeSharedActivator(IPacketSendBuffer<T>& tx_buffer,
                                            IPacketReceiveBuffer<T>& rx_buffer,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<T>>& data_available,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status)
                                : Socket<T>(tx_buffer, rx_buffer, tx_empty, data_available, connection_status)
                        {
                        }

                };

                std::shared_ptr<ISocket> s = std::make_shared<MakeSharedActivator>(tx_buffer,
                                                                                   rx_buffer,
                                                                                   tx_empty,
                                                                                   data_available,
                                                                                   connection_status);

                SocketDispatcher::instance().socket_created(s);
                return s;
            }

            template<typename T>
            Socket<T>::Socket(IPacketSendBuffer<T>& tx_buffer, IPacketReceiveBuffer<T>& rx_buffer,
                              smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                              smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<T>>& data_available,
                              smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status
            )
                    :
                    tx_buffer(tx_buffer),
                    rx_buffer(rx_buffer),
                    data_available(data_available),
                    tx_empty(tx_empty),
                    connection_status(connection_status)
            {
            }

            template<typename T>
            bool Socket<T>::start(std::shared_ptr<InetAddress> ip)
            {
                bool res = false;
                if (!started)
                {
                    this->ip = ip;
                    res = ip->is_valid() && create_socket();
                    if (res)
                    {
                        SocketDispatcher::instance().start_socket(shared_from_this());
                    }
                }

                return res;
            }

            template<typename T>
            bool Socket<T>::restart()
            {
                stop();
                return start(ip);
            }

            template<typename T>
            bool Socket<T>::create_socket()
            {
                bool res = false;

                if (socket_id < 0)
                {
                    socket_id = socket(ip->get_protocol_family(), SOCK_STREAM, 0);

                    if (socket_id == -1)
                    {
                        loge("Failed to create socket");
                    }
                    else
                    {
                        res = set_non_blocking();
                        int no_delay = 1;
                        res &= setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(no_delay)) == 0;
                        if (res)
                        {
                            log("Created socket");
                        }
                        else
                        {
                            loge("Failed to set socket options");
                        }
                    }
                }
                else
                {
                    res = true;
                }

                return res;
            }

            template<typename T>
            bool Socket<T>::set_non_blocking()
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

            template<typename T>
            void Socket<T>::readable()
            {
                // Detect disconnection
                char b[1];
                int res = recv(socket_id, b, 1, MSG_PEEK);
                if (res <= 0)
                {
                    if (res == -1)
                    {
                        loge("Disconnection detected");
                    }

                    stop();
                }
                else
                {
                    if (!rx_buffer.is_full())
                    {
                        // How much data to assemble the current packet?
                        int wanted_length = rx_buffer.amount_wanted();

                        // Try to read the desired amount
                        read_data(rx_buffer.get_write_pos(), wanted_length);
                    }
                }
            }

            template<typename T>
            void Socket<T>::writable()
            {
                // Any data to send?
                if (tx_buffer.is_empty())
                {
                    // Let the application know it may send a packet.
                    smooth::core::network::TransmitBufferEmptyEvent msg(shared_from_this());
                    tx_empty.push(msg);
                }
                else
                {
                    if (!tx_buffer.is_in_progress())
                    {
                        tx_buffer.prepare_next_packet();
                    }

                    if (tx_buffer.is_in_progress())
                    {
                        // Try to send as much as possible. The only guarantee POSIX gives when a socket is writable
                        // is that send( id, some_data, some_length ) will be >= 1 and may or may not send the entire
                        // packet.
                        write_data(tx_buffer.get_data_to_send(), tx_buffer.get_remaining_data_length());
                    }
                }
            }

            template<typename T>
            void Socket<T>::read_data(uint8_t* target, int max_length)
            {
                // Try to read the desired amount
                int read_count = recv(socket_id, target, max_length, 0);

                if (read_count == -1)
                {
                    if (errno != EWOULDBLOCK)
                    {
                        loge("Error during receive");
                        stop();
                    }
                }
                else
                {
                    rx_buffer.data_received(read_count);
                    if (rx_buffer.is_error())
                    {
                        log("Assembly error");
                        stop();
                    }
                    else if (rx_buffer.is_packet_complete())
                    {
                        DataAvailableEvent<T> d(&rx_buffer);
                        data_available.push(d);
                        rx_buffer.prepare_new_packet();
                    }
                }

            }

            template<typename T>
            void Socket<T>::write_data(const uint8_t* src, int length)
            {
                int amount_sent = send(socket_id, tx_buffer.get_data_to_send(), tx_buffer.get_remaining_data_length(),
                                       0);

                if (amount_sent == -1)
                {
                    loge("Failure during send");
                    stop();
                }
                else
                {
                    tx_buffer.data_has_been_sent(amount_sent);

                    // Was a complete packet sent?
                    if (!tx_buffer.is_in_progress())
                    {
                        // Let the application know it may now send another packet.
                        smooth::core::network::TransmitBufferEmptyEvent msg(shared_from_this());
                        tx_empty.push(msg);
                    }
                }
            }


            template<typename T>
            bool Socket<T>::is_started()
            {
                return started;
            }

            template<typename T>
            void Socket<T>::internal_start()
            {
                if (!is_started())
                {
                    bool could_create = create_socket();

                    if (could_create)
                    {
                        // The socket is non-blocking so we expect return value of either 0, or -1 with errno == EINPROGRESS
                        log("Connecting");
                        int res = connect(socket_id, ip->get_socket_address(), ip->get_socket_address_length());
                        if (res == 0 || (res == -1 && errno == EINPROGRESS))
                        {
                            started = true;
                        }
                        else
                        {
                            loge("Error during connect");
                        }
                    }

                    if (!started)
                    {
                        stop();
                    }
                }
            }

            template<typename T>
            bool Socket<T>::check_if_connection_is_completed()
            {
                int res = connect(socket_id, ip->get_socket_address(), ip->get_socket_address_length());
                if (res == -1 && errno == EISCONN)
                {
                    if (prepare_connected_socket())
                    {
                        connected = true;
                    }
                    else
                    {
                        stop();
                    }
                }

                return connected;
            }

            template<typename T>
            bool Socket<T>::is_active()
            {
                return started;
            }

            template<typename T>
            void Socket<T>::stop()
            {
                if (started && socket_id >= 0)
                {
                    log("Stopping");
                    shutdown(socket_id, SHUT_RDWR);
                    close(socket_id);
                    socket_id = -1;
                    started = false;
                    connected = false;
                    tx_buffer.clear();
                    rx_buffer.clear();
                    SocketDispatcher::instance().initiate_shutdown(shared_from_this());
                }
            }

            template<typename T>
            void Socket<T>::publish_connected_status(std::shared_ptr<ISocket>& socket)
            {
                if (is_connected())
                {
                    log("Connected");
                }
                else
                {
                    log("Disconnected");
                }

                ConnectionStatusEvent ev(socket, is_connected());
                connection_status.push(ev);
            }

            template<typename T>
            void Socket<T>::log(const char* message)
            {
                ESP_LOGV("Socket", "[%s, %d, %p]: %s", ip->get_address_as_string().c_str(), ip->get_port(), this, message);
            }

            template<typename T>
            void Socket<T>::loge(const char* message)
            {
                ESP_LOGE("Socket", "[%s, %d, %p]: %s: %s", ip->get_address_as_string().c_str(), ip->get_port(), this, message, strerror(errno));
            }
        }
    }
}
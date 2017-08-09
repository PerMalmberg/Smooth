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

            /// Socket is used to perform TCP/IP communication.
            /// \tparam Packet The type of the packet used for communication on this socket
            template<typename Packet>
            class Socket
                    : public ISocket, public std::enable_shared_from_this<ISocket>
            {
                public:
                    friend class smooth::core::network::SocketDispatcher;

                    /// Creates a socket for network communication, with the specified packet type.
                    /// \param tx_buffer The transmit buffer where outgoing packets are put by the application.
                    /// \param rx_buffer The receive buffer used when receiving data
                    /// \param tx_empty The response queue onto which events are put when all outgoing packets are sent.
                    /// These events are forwarded to the application via the response method.
                    /// \param data_available The response queue onto which events are put when data is available. These
                    /// These events are forwarded to the application via the response method.
                    /// \param connection_status The response queue into which events are put when a change in the connection
                    /// state is detected. These events are forwarded to the application via the response method.
                    /// \return a std::shared_ptr pointing to an instance of a ISocket object, or nullptr if no socket could be
                    /// created.
                    static std::shared_ptr<ISocket>
                    create(IPacketSendBuffer<Packet>& tx_buffer, IPacketReceiveBuffer<Packet>& rx_buffer,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status);

                    virtual ~Socket()
                    {
                        log("Destructing");
                    }

                    bool start(std::shared_ptr<InetAddress> ip) override;
                    void stop() override;
                    bool restart() override;

                    bool is_active() override;

                    void readable() override;

                    void writable() override;

                protected:
                    Socket(IPacketSendBuffer<Packet>& tx_buffer, IPacketReceiveBuffer<Packet>& rx_buffer,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status);

                    virtual bool create_socket();

                    virtual void read_data(uint8_t* target, int max_length);

                    virtual void write_data(const uint8_t* src, int length);

                    int get_socket_id() override
                    {
                        return socket_id;
                    }

                    IPacketSendBuffer<Packet>& tx_buffer;
                    IPacketReceiveBuffer<Packet>& rx_buffer;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty;
                private:

                    bool internal_start() override;

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
                    int socket_id = -1;
                    std::shared_ptr<InetAddress> ip;
                    bool started = false;
                    bool connected = false;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status;
            };


            template<typename Packet>
            std::shared_ptr<ISocket> Socket<Packet>::create(IPacketSendBuffer<Packet>& tx_buffer,
                                                            IPacketReceiveBuffer<Packet>& rx_buffer,
                                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status)
            {

                // This class is solely used to enabled access to the protected Socket<Packet> constructor from std::make_shared<>
                class MakeSharedActivator
                        : public Socket<Packet>
                {
                    public:
                        MakeSharedActivator(IPacketSendBuffer<Packet>& tx_buffer,
                                            IPacketReceiveBuffer<Packet>& rx_buffer,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status)
                                : Socket<Packet>(tx_buffer, rx_buffer, tx_empty, data_available, connection_status)
                        {
                        }

                };

                std::shared_ptr<ISocket> s = std::make_shared<MakeSharedActivator>(tx_buffer,
                                                                                   rx_buffer,
                                                                                   tx_empty,
                                                                                   data_available,
                                                                                   connection_status);
                return s;
            }

            template<typename Packet>
            Socket<Packet>::Socket(IPacketSendBuffer<Packet>& tx_buffer, IPacketReceiveBuffer<Packet>& rx_buffer,
                                   smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                   smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
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

            template<typename Packet>
            bool Socket<Packet>::start(std::shared_ptr<InetAddress> ip)
            {
                bool res = false;
                if (!started)
                {
                    this->ip = ip;
                    res = ip->is_valid();
                    if (res)
                    {
                        SocketDispatcher::instance().start_socket(shared_from_this());
                    }
                }

                return res;
            }

            template<typename Packet>
            bool Socket<Packet>::restart()
            {
                stop();
                return start(ip);
            }

            template<typename Packet>
            bool Socket<Packet>::create_socket()
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

            template<typename Packet>
            bool Socket<Packet>::set_non_blocking()
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

            template<typename Packet>
            void Socket<Packet>::readable()
            {
                if (!rx_buffer.is_full())
                {
                    // How much data to assemble the current packet?
                    int wanted_length = rx_buffer.amount_wanted();

                    // Try to read the desired amount
                    read_data(rx_buffer.get_write_pos(), wanted_length);
                }
            }

            template<typename Packet>
            void Socket<Packet>::writable()
            {
                if (!connected && socket_id >= 0)
                {
                    // Just connected
                    connected = true;
                    auto self = shared_from_this();
                    publish_connected_status(self);
                }

                if (connected)
                {
                    // Any data to send?
                    if (tx_buffer.is_empty())
                    {
                        // Let the application know it may send a packet.
                        smooth::core::network::TransmitBufferEmptyEvent event(shared_from_this());
                        tx_empty.push(event);
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
            }

            template<typename Packet>
            void Socket<Packet>::read_data(uint8_t* target, int max_length)
            {
                // Try to read the desired amount
                int read_count = recv(socket_id, target, max_length, 0);

                if (read_count == 0)
                {
                    // Disconnected
                    loge("Disconnection detected");
                    stop();
                }
                else if (read_count == -1)
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
                        DataAvailableEvent<Packet> d(&rx_buffer);
                        data_available.push(d);
                        rx_buffer.prepare_new_packet();
                    }
                }

            }

            template<typename Packet>
            void Socket<Packet>::write_data(const uint8_t* src, int length)
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
                        smooth::core::network::TransmitBufferEmptyEvent event(shared_from_this());
                        tx_empty.push(event);
                    }
                }
            }


            template<typename Packet>
            bool Socket<Packet>::internal_start()
            {
                if (!is_active())
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

                return started;
            }

            template<typename Packet>
            bool Socket<Packet>::is_active()
            {
                return started;
            }

            template<typename Packet>
            void Socket<Packet>::stop()
            {
                if (started)
                {
                    log("Stopping");
                    started = false;
                    connected = false;
                    tx_buffer.clear();
                    rx_buffer.clear();
                    SocketDispatcher::instance().shutdown_socket(shared_from_this());
                }
            }

            template<typename Packet>
            void Socket<Packet>::publish_connected_status(std::shared_ptr<ISocket>& socket)
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

            template<typename Packet>
            void Socket<Packet>::log(const char* message)
            {
                ESP_LOGV("Socket", "[%s, %d, %d, %p]: %s", ip->get_ip_as_string().c_str(), ip->get_port(), socket_id,
                         this, message);
            }

            template<typename Packet>
            void Socket<Packet>::loge(const char* message)
            {
                ESP_LOGE("Socket", "[%s, %d, %d %p]: %s: %s", ip->get_ip_as_string().c_str(), ip->get_port(), socket_id,
                         this,
                         message, strerror(errno));
            }
        }
    }
}
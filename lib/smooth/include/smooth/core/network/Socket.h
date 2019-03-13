//
// Created by permal on 6/25/17.
//

#pragma once

#include <sys/socket.h>
#include "InetAddress.h"
#include "ISocket.h"
#include <cstring>
#include <memory>
#include <chrono>
#include <smooth/core/util/CircularBuffer.h>
#include <smooth/core/timer/ElapsedTime.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/network/TransmitBufferEmptyEvent.h>
#include <smooth/core/network/DataAvailableEvent.h>
#include <smooth/core/network/PacketSendBuffer.h>
#include <smooth/core/network/SocketDispatcher.h>
#include <smooth/core/network/ConnectionStatusEvent.h>
#include <smooth/core/logging/log.h>

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
                    /// \param send_timeout The amount of time to wait for outgoing data to actually be sent to remote
                    /// endpoint (i.e. the maximum time between send() being called and the socket being writable again).
                    /// If this time is exceeded, the socket will be closed.
                    /// \return a std::shared_ptr pointing to an instance of a ISocket object, or nullptr if no socket could be
                    /// created.
                    static std::shared_ptr<ISocket>
                    create(IPacketSendBuffer<Packet>& tx_buffer, IPacketReceiveBuffer<Packet>& rx_buffer,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                           std::chrono::milliseconds send_timeout = std::chrono::milliseconds(1500));

                    ~Socket() override
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
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                           std::chrono::milliseconds send_timeout);

                    virtual bool create_socket();

                    virtual void read_data();

                    virtual void write_data();

                    void send_next_packet();

                    bool signal_new_connection();

                    int get_socket_id() override
                    {
                        return socket_id;
                    }

                    bool has_send_expired() const override
                    {
                        return elapsed_send_time.is_running()
                               && elapsed_send_time.get_running_time() > send_timeout;
                    }

                    IPacketSendBuffer<Packet>& tx_buffer;
                    IPacketReceiveBuffer<Packet>& rx_buffer;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty;

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

                    void publish_connected_status() override;

                    int socket_id = INVALID_SOCKET;
                    std::shared_ptr<InetAddress> ip;
                    smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status;

                    void stop_internal() override;

                    void clear_socket_id() override;

                    std::chrono::milliseconds send_timeout;

                    smooth::core::timer::ElapsedTime elapsed_send_time{};

                private:
                    bool started = false;
                    bool connected = false;
            };


            template<typename Packet>
            std::shared_ptr<ISocket> Socket<Packet>::create(IPacketSendBuffer<Packet>& tx_buffer,
                                                            IPacketReceiveBuffer<Packet>& rx_buffer,
                                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                                                            std::chrono::milliseconds send_timeout)
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
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                                            std::chrono::milliseconds send_timeout)
                                : Socket<Packet>(tx_buffer, rx_buffer, tx_empty, data_available, connection_status,
                                                 send_timeout)
                        {
                        }

                };

                std::shared_ptr<ISocket> s = std::make_shared<MakeSharedActivator>(tx_buffer,
                                                                                   rx_buffer,
                                                                                   tx_empty,
                                                                                   data_available,
                                                                                   connection_status,
                                                                                   send_timeout);
                return s;
            }

            template<typename Packet>
            Socket<Packet>::Socket(IPacketSendBuffer<Packet>& tx_buffer, IPacketReceiveBuffer<Packet>& rx_buffer,
                                   smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                   smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                   smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                                   std::chrono::milliseconds send_timeout
            )
                    :
                    tx_buffer(tx_buffer),
                    rx_buffer(rx_buffer),
                    data_available(data_available),
                    tx_empty(tx_empty),
                    connection_status(connection_status),
                    send_timeout(send_timeout)
            {
            }

            template<typename Packet>
            bool Socket<Packet>::start(std::shared_ptr<InetAddress> ip)
            {
                bool res = false;
                if (!started)
                {
                    elapsed_send_time.stop_and_zero();
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

                    if (socket_id == INVALID_SOCKET)
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
                if (started && !rx_buffer.is_full())
                {
                    read_data();
                }
            }

            template<typename Packet>
            void Socket<Packet>::writable()
            {
                if (started)
                {
                    elapsed_send_time.stop_and_zero();

                    if (signal_new_connection())
                    {
                        send_next_packet();
                    }
                }
            }

            template<typename Packet>
            bool Socket<Packet>::signal_new_connection()
            {
                if (!connected && socket_id >= 0)
                {
                    // Just connected
                    connected = true;
                    publish_connected_status();
                }

                return connected;
            }

            template<typename Packet>
            void Socket<Packet>::send_next_packet()
            {
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
                            write_data();
                        }
                    }
                }
            }

            template<typename Packet>
            void Socket<Packet>::read_data()
            {
                errno = 0;

                // How much data to assemble the current packet?
                int wanted_length = rx_buffer.amount_wanted();

                // Try to read the desired amount
                auto read_count = recv(socket_id, rx_buffer.get_write_pos(), static_cast<size_t>(wanted_length), 0);

                if (read_count == -1)
                {
                    if (errno != EWOULDBLOCK)
                    {
                        loge("Error during receive");
                        stop();
                    }
                }
                else if (read_count > 0)
                {
                    rx_buffer.data_received(static_cast<int>(read_count));
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
            void Socket<Packet>::write_data()
            {
                // Try to send as much as possible. The only guarantee POSIX gives when a socket is writable
                // is that send( id, some_data, some_length ) will be >= 1 and may or may not send the entire
                // packet.
                auto data_to_send = tx_buffer.get_data_to_send();
                auto length = tx_buffer.get_remaining_data_length();
                auto amount_sent = send(socket_id,
                                        data_to_send,
                                        static_cast<size_t>(length),
                                        SEND_FLAGS);

                if (amount_sent == -1)
                {
                    loge("Failure during send");
                    stop();
                }
                else
                {
                    tx_buffer.data_has_been_sent(static_cast<int>(amount_sent));

                    // Was a complete packet sent?
                    if (tx_buffer.is_in_progress())
                    {
                        elapsed_send_time.start();
                    }
                    else
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
                stop_internal();
                SocketDispatcher::instance().perform_op(SocketOperation::Op::Stop, shared_from_this());
            }

            template<typename Packet>
            void Socket<Packet>::stop_internal()
            {
                if (started)
                {
                    log("Stopping");
                    started = false;
                    connected = false;
                    tx_buffer.clear();
                    rx_buffer.clear();
                    elapsed_send_time.stop_and_zero();
                }
            }

            template<typename Packet>
            void Socket<Packet>::publish_connected_status()
            {
                if (is_connected())
                {
                    log("Connected");
                }
                else
                {
                    log("Disconnected");
                }

                auto self = shared_from_this();
                ConnectionStatusEvent ev(self, is_connected());
                connection_status.push(ev);
            }

            template<typename Packet>
            void Socket<Packet>::clear_socket_id()
            {
                socket_id = INVALID_SOCKET;
            }

            template<typename Packet>
            void Socket<Packet>::log(const char* message)
            {
                Log::verbose("Socket",
                             Format("[{1}, {2}, {3}, {4}]: {5}",
                                    Str(ip->get_host()),
                                    Int32(ip->get_port()),
                                    Int32(socket_id),
                                    Pointer(this),
                                    Str(message)));
            }

            template<typename Packet>
            void Socket<Packet>::loge(const char* message)
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
        }
    }
}
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

#include "InetAddress.h"
#include "ISocket.h"
#include <cstring>
#include <memory>
#include <chrono>
#include "CommonSocket.h"
#include "ServerClient.h"
#include "BufferContainer.h"
#include "smooth/core/util/CircularBuffer.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/network/event/TransmitBufferEmptyEvent.h"
#include "smooth/core/network/event/DataAvailableEvent.h"
#include "smooth/core/network/PacketSendBuffer.h"
#include "smooth/core/network/SocketDispatcher.h"
#include "smooth/core/network/event/ConnectionStatusEvent.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/util/create_protected.h"

namespace smooth::core::network
{
    static constexpr const std::chrono::milliseconds DefaultSendTimeout{ 1500 };
    static constexpr const std::chrono::milliseconds DefaultReceiveTimeout{ 1500 };

    // Depending on if Smooth is compiled using regular gcc or xtensa-gcc,
    // recv() returns different types. As such we need to cast the return value of that type.
    template<typename T>
    inline int socket_cast(T t)
    {
#ifdef ESP_PLATFORM

        return t;
#else

        return static_cast<int>(t);
#endif
    }

    /// Socket is used to perform TCP/IP communication.
    /// \tparam Packet The type of the packet used for communication on this socket
    template<typename Protocol, typename Packet = typename Protocol::packet_type>
    class Socket
        : public CommonSocket
    {
        public:
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
            static std::shared_ptr<Socket<Protocol>>
            create(std::weak_ptr<BufferContainer<Protocol>> buffer_container,
                   std::chrono::milliseconds send_timeout = DefaultSendTimeout,
                   std::chrono::milliseconds receive_timeout = DefaultReceiveTimeout);

            static std::shared_ptr<Socket<Protocol>>
            create(std::shared_ptr<smooth::core::network::InetAddress> ip,
                   int socket_id,
                   std::weak_ptr<BufferContainer<Protocol>> buffer_container,
                   std::chrono::milliseconds send_timeout = DefaultSendTimeout,
                   std::chrono::milliseconds receive_timeout = DefaultReceiveTimeout);

            ~Socket() override = default;

            bool start(std::shared_ptr<InetAddress> ip) override;

            void readable(ISocketBackOff& ops) override;

            void writable() override;

            virtual void set_existing_socket(const std::shared_ptr<InetAddress>& address, int socket_id);

            bool send(const Packet& packet);

            bool is_server() const override
            {
                return false;
            }

        protected:
            Socket(std::weak_ptr<BufferContainer<Protocol>> buffer_container);

            virtual bool create_socket();

            virtual void read_data(const std::shared_ptr<BufferContainer<Protocol>>& container);

            virtual void write_data(const std::shared_ptr<BufferContainer<Protocol>>& container);

            void send_next_packet();

            bool signal_new_connection();

            bool internal_start() override;

            bool has_data_to_transmit() override
            {
                // Also check on connected state so that we don't try to send data
                // when the socket just has been closed while in SocketDispatcher::tick()
                bool res = connected;

                if (res)
                {
                    auto cont = get_container_or_close();

                    if (cont)
                    {
                        res = !cont->get_tx_buffer().is_empty();
                    }
                }

                return res;
            }

            void publish_connected_status() override;

            void stop_internal() override;

            std::shared_ptr<BufferContainer<Protocol>> get_container_or_close();

            bool set_no_delay();

            std::weak_ptr<BufferContainer<Protocol>> buffers{};
        private:
            void clear_buffers();
    };

    template<typename Protocol, typename Packet>
    std::shared_ptr<Socket<Protocol>> Socket<Protocol, Packet>::create(
        std::shared_ptr<smooth::core::network::InetAddress> ip,
        int socket_id,
        std::weak_ptr<BufferContainer<Protocol>> buffer_container,
        std::chrono::milliseconds send_timeout,
        std::chrono::milliseconds receive_timeout)
    {
        auto s = create(buffer_container, send_timeout, receive_timeout);
        s->set_send_timeout(send_timeout);
        s->set_receive_timeout(receive_timeout);
        s->set_existing_socket(ip, socket_id);

        return s;
    }

    template<typename Protocol, typename Packet>
    std::shared_ptr<Socket<Protocol>> Socket<Protocol, Packet>::create(
        std::weak_ptr<BufferContainer<Protocol>> buffer_container,
        std::chrono::milliseconds send_timeout,
        std::chrono::milliseconds receive_timeout)
    {
        auto s = smooth::core::util::create_protected_shared<Socket<Protocol, Packet>>(buffer_container);
        s->set_send_timeout(send_timeout);
        s->set_receive_timeout(receive_timeout);

        return s;
    }

    template<typename Protocol, typename Packet>
    Socket<Protocol, Packet>::Socket(std::weak_ptr<BufferContainer<Protocol>> buffer_container)
            : CommonSocket(),
              buffers(std::move(buffer_container))
    {
        // In case the buffers have been used by another socket previously (i.e. by another ServerClient),
        // we make sure they are empty before we start using them.
        clear_buffers();
    }

    template<typename Protocol, typename Packet>
    bool Socket<Protocol, Packet>::start(std::shared_ptr<InetAddress> ip)
    {
        bool res = false;

        if (!active)
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

    template<typename Protocol, typename Packet>
    bool Socket<Protocol, Packet>::create_socket()
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
                res = set_non_blocking() && set_no_delay();
            }
        }
        else
        {
            res = true;
        }

        return res;
    }

    template<typename Protocol, typename Packet>
    bool Socket<Protocol, Packet>::set_no_delay()
    {
        int no_delay = 1;
        auto res = setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(no_delay)) == 0;

        if (!res)
        {
            loge("Failed to set no delay socket option");
        }

        return res;
    }

    template<typename Protocol, typename Packet>
    void Socket<Protocol, Packet>::readable(ISocketBackOff&)
    {
        if (is_active())
        {
            this->elapsed_receive_time.start();

            auto cont = get_container_or_close();

            if (cont)
            {
                if (!cont->get_rx_buffer().is_full())
                {
                    read_data(cont);
                }
            }
        }
    }

    template<typename Protocol, typename Packet>
    void Socket<Protocol, Packet>::writable()
    {
        if (is_active() && signal_new_connection())
        {
            elapsed_send_time.stop_and_zero();
            send_next_packet();
        }
    }

    template<typename Protocol, typename Packet>
    bool Socket<Protocol, Packet>::signal_new_connection()
    {
        if (!connected && socket_id >= 0)
        {
            // Just connected
            connected = true;
            publish_connected_status();
        }

        return connected;
    }

    template<typename Protocol, typename Packet>
    void Socket<Protocol, Packet>::send_next_packet()
    {
        if (connected)
        {
            auto cont = get_container_or_close();

            if (cont)
            {
                auto& tx = cont->get_tx_buffer();

                // Any data to send?
                if (tx.is_empty())
                {
                    // Let the application know it may send a packet.
                    smooth::core::network::event::TransmitBufferEmptyEvent event(shared_from_this());
                    cont->get_tx_empty()->push(event);
                }
                else
                {
                    if (!tx.is_in_progress())
                    {
                        tx.prepare_next_packet();
                    }

                    if (tx.is_in_progress())
                    {
                        write_data(cont);
                    }
                }
            }
        }
    }

    template<typename Protocol, typename Packet>
    void Socket<Protocol, Packet>::read_data(const std::shared_ptr<BufferContainer<Protocol>>& container)
    {
        auto& rx = container->get_rx_buffer();

        // How much data to assemble the current packet?
        int wanted_length = rx.amount_wanted();

        // Try to read the desired amount
        ssize_t read_count = 0;
        {
            auto write_pos = rx.get_write_pos();
            read_count = recv(socket_id, static_cast<void*>(write_pos), static_cast<size_t>(wanted_length), 0);
        }

        if (read_count == 0)
        {
            stop("Underlying socket closed (recv returned 0)");
        }
        else if (read_count < 0)
        {
            if (errno != EWOULDBLOCK)
            {
                stop("Error during receive");
            }
        }
        else
        {
            rx.data_received(socket_cast(read_count));

            if (rx.is_error())
            {
                rx.prepare_new_packet();
                stop("Assembly error");
            }
            else if (rx.is_packet_complete())
            {
                event::DataAvailableEvent<Protocol> d(&rx);
                container->get_data_available()->push(d);
                rx.prepare_new_packet();
            }
        }

        elapsed_receive_time.start();
    }

    template<typename Protocol, typename Packet>
    void Socket<Protocol, Packet>::write_data(const std::shared_ptr<BufferContainer<Protocol>>& container)
    {
        this->elapsed_receive_time.start();

        // Try to send as much as possible. The only guarantee POSIX gives when a socket is writable
        // is that send( id, some_data, some_length ) will be >= 1 and may or may not send the entire
        // packet.
        auto& tx = container->get_tx_buffer();
        auto data_to_send = tx.get_data_to_send();
        auto length = tx.get_remaining_data_length();
        auto amount_sent = ::send(socket_id,
                                  data_to_send,
                                  static_cast<size_t>(length),
                                  SEND_FLAGS);

        if (amount_sent == -1)
        {
            stop("Failure during send");
        }
        else
        {
            tx.data_has_been_sent(static_cast<int>(amount_sent));

            // Was a complete packet sent?
            if (tx.is_in_progress())
            {
                elapsed_send_time.start();
            }
            else
            {
                // Let the application know it may now send another packet.
                smooth::core::network::event::TransmitBufferEmptyEvent event(shared_from_this());
                container->get_tx_empty()->push(event);
            }
        }
    }

    template<typename Protocol, typename Packet>
    bool Socket<Protocol, Packet>::internal_start()
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
                    active = true;
                }
                else
                {
                    loge("Error during connect");
                }
            }

            if (!active)
            {
                stop("Not active");
            }
        }

        return active;
    }

    template<typename Protocol, typename Packet>
    void Socket<Protocol, Packet>::stop_internal()
    {
        if (active)
        {
            log("Socket stopping");
            active = false;
            connected = false;
            elapsed_send_time.stop_and_zero();
        }
    }

    template<typename Protocol, typename Packet>
    void Socket<Protocol, Packet>::publish_connected_status()
    {
        if (is_connected())
        {
            log("Connected");
        }
        else
        {
            log("Disconnected");
        }

        auto cont = get_container_or_close();

        if (cont)
        {
            cont->get_connection_status()->push(event::ConnectionStatusEvent(shared_from_this(), is_connected()));
        }
    }

    template<typename Protocol, typename Packet>
    void Socket<Protocol, Packet>::set_existing_socket(const std::shared_ptr<InetAddress>& address, int socket_id)
    {
        this->ip = address;
        this->socket_id = socket_id;
        active = true;
        connected = true;
        set_non_blocking();
        set_no_delay();

        SocketDispatcher::instance().perform_op(SocketOperation::Op::AddActiveSocket, shared_from_this());
    }

    template<typename Protocol, typename Packet>
    std::shared_ptr<BufferContainer<Protocol>> Socket<Protocol, Packet>::get_container_or_close()
    {
        auto cont = buffers.lock();

        if (!cont)
        {
            // If the buffer container has expired, it means the client
            // has let it go, meaning this socket also can close.
            stop("Could not get buffer container");
        }

        return cont;
    }

    template<typename Protocol, typename Packet>
    bool Socket<Protocol, Packet>::send(const Packet& packet)
    {
        bool res = false;
        auto cont = buffers.lock();

        if (cont)
        {
            res = cont->get_tx_buffer().put(packet);
        }

        return res;
    }

    template<typename Protocol, typename Packet>
    void Socket<Protocol, Packet>::clear_buffers()
    {
        auto cont = buffers.lock();

        if (cont)
        {
            cont->clear();
        }
    }
}

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
#include "Socket.h"
#include "MbedTLSContext.h"
#include <mbedtls/error.h>

namespace smooth::core::network
{
    static int ssl_recv(void* ctx, uint8_t* buf, size_t len)
    {
        auto socket = reinterpret_cast<ISocket*>(ctx);
        errno = 0;

        auto amount_received = socket_cast(recv(socket->get_socket_id(), buf, len, 0));

        if (amount_received < 0)
        {
            if (errno == EWOULDBLOCK)
            {
                amount_received = MBEDTLS_ERR_SSL_WANT_READ;
            }
        }

        return amount_received;
    }

    static int ssl_send(void* ctx, const uint8_t* buff, size_t len)
    {
        auto socket = reinterpret_cast<ISocket*>(ctx);
        errno = 0;

        int amount_sent = socket_cast(send(socket->get_socket_id(), buff, len, ISocket::SEND_FLAGS));

        if (amount_sent < 0)
        {
            if (errno == EWOULDBLOCK)
            {
                amount_sent = MBEDTLS_ERR_SSL_WANT_WRITE;
            }
        }
        else if (amount_sent < static_cast<int>(len))
        {
            // More data to send
            amount_sent = MBEDTLS_ERR_SSL_WANT_WRITE;
        }

        return amount_sent;
    }

    template<typename Protocol, typename Packet = typename Protocol::packet_type>
    class SecureSocket
        : public Socket<Protocol, Packet>
    {
        public:
            friend class smooth::core::network::SocketDispatcher;

            /// Creates a socket for network communication, with the specified packet type.
            /// \param send_timeout The amount of time to wait for outgoing data to actually be sent to remote
            /// endpoint (i.e. the maximum time between send() being called and the socket being writable again).
            /// If this time is exceeded, the socket will be closed.
            /// \return a std::shared_ptr pointing to an instance of a ISocket object, or nullptr if no socket could be
            /// created.
            static std::shared_ptr<SecureSocket<Protocol>>
            create(std::weak_ptr<BufferContainer<Protocol>> buffer_container,
                   std::unique_ptr<SSLContext> secure_context,
                   std::chrono::milliseconds send_timeout = std::chrono::milliseconds(5000),
                   std::chrono::milliseconds receive_timeout = std::chrono::milliseconds{ 0 });

            static std::shared_ptr<SecureSocket<Protocol>>
            create(std::shared_ptr<smooth::core::network::InetAddress> ip,
                   int socket_id,
                   std::weak_ptr<BufferContainer<Protocol>> buffer_container,
                   std::unique_ptr<SSLContext> secure_context,
                   std::chrono::milliseconds timeout = std::chrono::milliseconds(5000),
                   std::chrono::milliseconds receive_timeout = std::chrono::milliseconds{ 0 });

            void set_existing_socket(const std::shared_ptr<InetAddress>& address, int socket_id) override;

        protected:
            SecureSocket(std::weak_ptr<BufferContainer<Protocol>> buffer_container,
                         std::unique_ptr<SSLContext> context)
                    : Socket<Protocol, Packet>(std::move(buffer_container)),
                      secure_context(std::move(context))
            {
                mbedtls_ssl_set_bio(*secure_context,
            this,
            ssl_send,
            ssl_recv,
            nullptr);
            }

            void readable(ISocketBackOff& ops) override;

            void writable() override;

            void read_data(const std::shared_ptr<BufferContainer<Protocol>>& container) override;

            void write_data(const std::shared_ptr<BufferContainer<Protocol>>& container) override;

            bool has_data_to_transmit() override;

        private:
            static constexpr const char* tag = "SecureSocket";
            std::unique_ptr<SSLContext> secure_context{};

            bool is_handshake_complete(const SSLContext& ctx) const;

            void do_handshake_step();

            bool needs_tls_transfer(int code) const
            {
                return code == MBEDTLS_ERR_SSL_WANT_READ
                       || code == MBEDTLS_ERR_SSL_WANT_WRITE
#ifdef MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS
                       || code == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS
#endif
                ;
            }
    };

    template<typename Protocol, typename Packet>
    std::shared_ptr<SecureSocket<Protocol>> SecureSocket<Protocol, Packet>::create(
        std::shared_ptr<smooth::core::network::InetAddress> ip,
        int socket_id,
        std::weak_ptr<BufferContainer<Protocol>> buffer_container,
        std::unique_ptr<SSLContext> context,
        std::chrono::milliseconds send_timeout,
        std::chrono::milliseconds receive_timeout)
    {
        auto s = create(buffer_container, std::move(context));
        s->set_send_timeout(send_timeout);
        s->set_receive_timeout(receive_timeout);
        s->set_existing_socket(ip, socket_id);

        return s;
    }

    template<typename Protocol, typename Packet>
    std::shared_ptr<SecureSocket<Protocol>> SecureSocket<Protocol, Packet>::create(
        std::weak_ptr<BufferContainer<Protocol>> buffer_container,
        std::unique_ptr<SSLContext> context,
        std::chrono::milliseconds send_timeout,
        std::chrono::milliseconds receive_timeout)
    {
        auto s = smooth::core::util::create_protected_shared<SecureSocket<Protocol, Packet>>(buffer_container,
        std::move(context));
        s->set_send_timeout(send_timeout);
        s->set_receive_timeout(receive_timeout);

        return s;
    }

    template<typename Protocol, typename Packet>
    void SecureSocket<Protocol, Packet>::set_existing_socket(const std::shared_ptr<InetAddress>& address,
                                                             int socket_id)
    {
        Socket<Protocol, Packet>::set_existing_socket(address, socket_id);
    }

    template<typename Protocol, typename Packet>
    void SecureSocket<Protocol, Packet>::readable(ISocketBackOff& ops)
    {
        if (this->is_active())
        {
            this->elapsed_receive_time.start();

            if (is_handshake_complete(*secure_context))
            {
                Socket<Protocol, Packet>::readable(ops);
            }
            else
            {
                do_handshake_step();
            }
        }
    }

    template<typename Protocol, typename Packet>
    void SecureSocket<Protocol, Packet>::writable()
    {
        if (this->is_active() && this->signal_new_connection())
        {
            this->elapsed_send_time.start();

            if (is_handshake_complete(*secure_context))
            {
                Socket<Protocol, Packet>::writable();
            }
            else
            {
                do_handshake_step();
            }
        }
    }

    template<typename Protocol, typename Packet>
    void SecureSocket<Protocol, Packet>::read_data(const std::shared_ptr<BufferContainer<Protocol>>& container)
    {
        // How much data to assemble the current packet?

        auto& rx = container->get_rx_buffer();

        do
        {
            if (rx.is_full())
            {
                // Receiver is full. Since mbedtls_ssl_read() moves data from the underlying socket, we get no
                // no more readable-events on it, which results in the SocketDispatcher no longer calling
                // readable() on this socket, in turn preventing this function be called again to read the
                // remaining decrypted data. As such we have to wait a bit so the application consumes at
                // least one packet.
                std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });

                // Likely better solution: Set an artificial readable-indicator on the socket telling the
                // SocketDispatcher there are more data to be read, forcing a readable()-call on the socket.
            }
            else
            {
                int wanted_length = rx.amount_wanted();
                auto read_amount = 0;

                {
                    auto write_pos = rx.get_write_pos();
                    read_amount = mbedtls_ssl_read(*secure_context,
                                                   static_cast<uint8_t*>(write_pos),
                                                   static_cast<size_t>(wanted_length));
                }

                if (read_amount == 0)
                {
                    this->stop("Underlying socket closed (mbedtls_ssl_read returned 0)");
                }
                else if (read_amount < 0)
                {
                    if (!needs_tls_transfer(read_amount))
                    {
                        char buf[128];
                        mbedtls_strerror(read_amount, buf, sizeof(buf));
                        this->stop(buf);
                    }
                }
                else
                {
                    rx.data_received(read_amount);

                    if (rx.is_error())
                    {
                        Log::error(tag, "Assembly error");
                        rx.prepare_new_packet();
                        this->stop("Assembly error");
                    }
                    else if (rx.is_packet_complete())
                    {
                        event::DataAvailableEvent<Protocol> d(&rx);
                        container->get_data_available()->push(d);
                        rx.prepare_new_packet();
                    }
                }
            }
        }
        while (this->is_active()
               && mbedtls_ssl_get_bytes_avail(*secure_context) > 0);
    }

    template<typename Protocol, typename Packet>
    void SecureSocket<Protocol, Packet>::write_data(const std::shared_ptr<BufferContainer<Protocol>>& container)
    {
        this->elapsed_receive_time.start();

        auto& tx = container->get_tx_buffer();
        auto data_to_send = tx.get_data_to_send();

        auto length = tx.get_remaining_data_length();

        auto amount_sent = 0;

        do
        {
            amount_sent = mbedtls_ssl_write(*secure_context,
                                            data_to_send,
                                            static_cast<size_t>(length));

            if (!needs_tls_transfer(amount_sent))
            {
                if (amount_sent > 0)
                {
                    tx.data_has_been_sent(amount_sent);

                    // Was a complete packet sent?
                    if (tx.is_in_progress())
                    {
                        this->elapsed_send_time.start();
                    }
                    else
                    {
                        // Let the application know it may now send another packet.
                        event::TransmitBufferEmptyEvent event(this->shared_from_this());
                        container->get_tx_empty()->push(event);
                    }
                }

                if (amount_sent < 0 && !needs_tls_transfer(amount_sent))
                {
                    log_mbedtls_error("SecureSocket", "mbedtls_ssl_write", amount_sent);
                    this->stop("Error writing");
                }
            }
        }
        while (needs_tls_transfer(amount_sent));
    }

    template<typename Protocol, typename Packet>
    bool SecureSocket<Protocol, Packet>::is_handshake_complete(const SSLContext& ctx) const
    {
        return ctx.get_state() == MBEDTLS_SSL_HANDSHAKE_OVER;
    }

    template<typename Protocol, typename Packet>
    void SecureSocket<Protocol, Packet>::do_handshake_step()
    {
        this->elapsed_receive_time.start();
        this->elapsed_send_time.start();

        auto res = mbedtls_ssl_handshake_step(*secure_context);

        if (needs_tls_transfer(res))
        {
            // Handshake not yet complete
        }
        else if (res < 0)
        {
            // Handshake failed
            log_mbedtls_error("SecureSocket", "mbedtls_ssl_handshake_step", res);
            this->stop("Error during handshake");
        }
    }

    template<typename Protocol, typename Packet>
    bool SecureSocket<Protocol, Packet>::has_data_to_transmit()
    {
        return !is_handshake_complete(*secure_context)
               || Socket<Protocol, Packet>::has_data_to_transmit();
    }
}

#pragma once

#include <sys/socket.h>
#include "Socket.h"
#include "MbedTLSContext.h"
#include <mbedtls/error.h>

#ifdef ESP_PLATFORM
#define socket_cast(x) (x)
#else
#define socket_cast(x) static_cast<int>(x);
#endif

namespace smooth
{
    namespace core
    {
        namespace network
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
                    static std::shared_ptr<SecureSocket<Protocol>>
                    create(std::shared_ptr<BufferContainer<Protocol>> buffer_container,
                           std::chrono::milliseconds send_timeout = std::chrono::milliseconds(1500));

                    static std::shared_ptr<SecureSocket<Protocol>>
                    create(std::shared_ptr<smooth::core::network::InetAddress> ip,
                           int socket_id,
                           std::shared_ptr<BufferContainer<Protocol>> buffer_container,
                           std::chrono::milliseconds timeout);

                protected:
                    SecureSocket(std::shared_ptr<BufferContainer<Protocol>> buffer_container,
                                 std::chrono::milliseconds send_timeout)
                            : Socket<Protocol, Packet>(buffer_container, send_timeout)
                    {

                    }

                    bool create_socket() override;

                    bool internal_start() override;

                    void readable() override;

                    void writable() override;

                    void read_data() override;

                    void write_data() override;

                private:
                    static constexpr const char* tag = "SecureSocket";
                    std::unique_ptr<MBedTLSContext> secure_context{};

                    bool is_handshake_complete(const MBedTLSContext& ctx) const;


                    void do_handshake_step();
            };

            template<typename Protocol, typename Packet>
            std::shared_ptr<SecureSocket<Protocol>>
            SecureSocket<Protocol, Packet>::create(std::shared_ptr<smooth::core::network::InetAddress> ip,
                                             int socket_id,
                                             std::shared_ptr<BufferContainer<Protocol>> buffer_container,
                                             std::chrono::milliseconds timeout)
            {
                auto s = create(buffer_container, timeout);
                s->set_existing_socket(ip, socket_id);
                return s;
            }

            template<typename Protocol, typename Packet>
            std::shared_ptr<SecureSocket<Protocol>>
            SecureSocket<Protocol, Packet>::create(std::shared_ptr<BufferContainer<Protocol>> buffer_container,
                                                   std::chrono::milliseconds send_timeout)
            {
                // This class is solely used to enabled access to the protected SecureSocket<Protocol, Packet> constructor from std::make_shared<>
                class MakeSharedActivator
                        : public SecureSocket<Protocol, Packet>
                {
                    public:
                        MakeSharedActivator(std::shared_ptr<BufferContainer<Protocol>> buffer_container,
                                            std::chrono::milliseconds send_timeout)
                                : SecureSocket<Protocol, Packet>(buffer_container,
                                                                 send_timeout)
                        {
                        }

                };

                return std::make_shared<MakeSharedActivator>(buffer_container, send_timeout);
            }

            template<typename Protocol, typename Packet>
            bool SecureSocket<Protocol, Packet>::create_socket()
            {
                secure_context = std::make_unique<MBedTLSContext>();
                auto res = secure_context->init();

                if (res)
                {
                    res = Socket<Protocol, Packet>::create_socket();
                }

                return res;
            }

            template<typename Protocol, typename Packet>
            bool SecureSocket<Protocol, Packet>::internal_start()
            {
                auto res = Socket<Protocol, Packet>::internal_start();

                if (res)
                {
                    // At this point we *may* have a connected socket
                    mbedtls_ssl_set_bio(secure_context->get_context(), this, ssl_send, ssl_recv, nullptr);
                }

                return res;
            }

            template<typename Protocol, typename Packet>
            void SecureSocket<Protocol, Packet>::readable()
            {
                if (this->is_active() && this->is_connected())
                {
                    if (is_handshake_complete(*secure_context))
                    {
                        Socket<Protocol, Packet>::readable();
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
            void SecureSocket<Protocol, Packet>::read_data()
            {
                // How much data to assemble the current packet?
                auto cont = this->get_container_or_close();
                if (cont)
                {
                    auto& rx = cont->get_rx_buffer();
                    int wanted_length = rx.amount_wanted();

                    auto read_amount = mbedtls_ssl_read(secure_context->get_context(),
                                                        rx.get_write_pos(),
                                                        static_cast<size_t>(wanted_length));

                    if (read_amount == 0)
                    {
                        // Underlying socket closed
                        this->stop();
                    }
                    else if (read_amount < 0
                             && read_amount != MBEDTLS_ERR_SSL_WANT_READ
                             && read_amount != MBEDTLS_ERR_SSL_WANT_WRITE)
                    {
                        char buf[128];
                        mbedtls_strerror(read_amount, buf, sizeof(buf));
                        Log::error(tag, Format("mbedtls_ssl_read failed: {1}", Str(buf)));
                        this->stop();
                    }
                    else
                    {
                        rx.data_received(read_amount);
                        if (rx.is_error())
                        {
                            Log::error(tag, "Assembly error");
                            this->stop();
                        }
                        else if (rx.is_packet_complete())
                        {
                            event::DataAvailableEvent<Protocol> d(&rx);
                            cont->get_data_available().push(d);
                            rx.prepare_new_packet();
                        }
                    }
                }
            }

            template<typename Protocol, typename Packet>
            void SecureSocket<Protocol, Packet>::write_data()
            {
                auto cont = this->get_container_or_close();
                if (cont)
                {
                    auto& tx = cont->get_tx_buffer();
                    auto data_to_send = tx.get_data_to_send();

                    auto length = tx.get_remaining_data_length();

                    auto amount_sent = mbedtls_ssl_write(secure_context->get_context(),
                                                         data_to_send,
                                                         static_cast<size_t>(length));

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
                            cont->get_tx_empty().push(event);
                        }
                    }

                    if (amount_sent < 0
                        && amount_sent != MBEDTLS_ERR_SSL_WANT_READ
                        && amount_sent != MBEDTLS_ERR_SSL_WANT_WRITE)
                    {
                        char buf[128];
                        mbedtls_strerror(amount_sent, buf, sizeof(buf));
                        Log::error(tag, Format("mbedtls_ssl_write failed: {1}", Str(buf)));
                        this->stop();
                    }
                }
            }

            template<typename Protocol, typename Packet>
            bool SecureSocket<Protocol, Packet>::is_handshake_complete(const MBedTLSContext& ctx) const
            {
                return ctx.get_context()->state == MBEDTLS_SSL_HANDSHAKE_OVER;
            }

            template<typename Protocol, typename Packet>
            void SecureSocket<Protocol, Packet>::do_handshake_step()
            {
                auto res = mbedtls_ssl_handshake_step(secure_context->get_context());

                if (res == MBEDTLS_ERR_SSL_WANT_READ
                    || res == MBEDTLS_ERR_SSL_WANT_WRITE)
                {
                    // Handshake not yet complete
                }
                else if (res < 0)
                {
                    // Handshake failed
                    char buf[128];
                    mbedtls_strerror(res, buf, sizeof(buf));
                    Log::error(tag, Format("mbedtls_ssl_handshake failed: {1}", Str(buf)));
                    this->stop();
                }
            }
        }
    }
}
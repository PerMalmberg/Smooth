#pragma once

#include <sys/socket.h>
#include "Socket.h"
#include "MbedTLSContext.h"
#include <mbedtls/error.h>


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

                auto res = recv(socket->get_socket_id(), buf, len, 0);

                if (res < 0)
                {
                    if (errno == EWOULDBLOCK)
                    {
                        res = MBEDTLS_ERR_SSL_WANT_READ;
                    }
                    else
                    {
                        res = -1;
                    }
                }

                return static_cast<int>(res);
            }

            static int ssl_send(void* ctx, const uint8_t* buff, size_t len)
            {
                auto socket = reinterpret_cast<ISocket*>(ctx);
                errno = 0;

                auto amount_sent = send(socket->get_socket_id(), buff, len, ISocket::SEND_FLAGS);

                if (amount_sent < 0)
                {
                    // Error
                }
                else if (amount_sent < len)
                {
                    // More data to send
                    amount_sent = MBEDTLS_ERR_SSL_WANT_WRITE;
                }

                return static_cast<int>(amount_sent);
            }


            template<typename Packet>
            class SecureSocket
                    : public Socket<Packet>
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

                protected:
                    SecureSocket(IPacketSendBuffer<Packet>& tx_buffer, IPacketReceiveBuffer<Packet>& rx_buffer,
                                 smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                 smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                 smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                                 std::chrono::milliseconds send_timeout)
                            : Socket<Packet>(tx_buffer, rx_buffer, tx_empty, data_available, connection_status,
                                             send_timeout)
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
                    MBedTLSContext secure_context{};


            };

            template<typename Packet>
            std::shared_ptr<ISocket> SecureSocket<Packet>::create(IPacketSendBuffer<Packet>& tx_buffer,
                                                                  IPacketReceiveBuffer<Packet>& rx_buffer,
                                                                  smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                                                  smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                                                  smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                                                                  std::chrono::milliseconds send_timeout)
            {

                // This class is solely used to enabled access to the protected SecureSocket<Packet> constructor from std::make_shared<>
                class MakeSharedActivator
                        : public SecureSocket<Packet>
                {
                    public:
                        MakeSharedActivator(IPacketSendBuffer<Packet>& tx_buffer,
                                            IPacketReceiveBuffer<Packet>& rx_buffer,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status,
                                            std::chrono::milliseconds send_timeout)
                                : SecureSocket<Packet>(tx_buffer, rx_buffer, tx_empty, data_available,
                                                       connection_status,
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
            bool SecureSocket<Packet>::create_socket()
            {
                auto res = secure_context.init();
                if (res)
                {
                    res = Socket<Packet>::create_socket();
                }

                return res;
            }

            template<typename Packet>
            bool SecureSocket<Packet>::internal_start()
            {
                auto res = Socket<Packet>::internal_start();

                if (res)
                {
                    // At this point we have a connected socket
                    mbedtls_ssl_set_bio(secure_context.get_context(), this, ssl_send, ssl_recv, nullptr);

                }

                return res;
            }

            template<typename Packet>
            void SecureSocket<Packet>::readable()
            {
                auto res = mbedtls_ssl_handshake(secure_context.get_context());

                if (res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE)
                {
                    // Handshake not yet complete
                }
                else if (res == 0)
                {
                    // Handshake is complete, read application data.
                    Socket<Packet>::readable();
                }
                else
                {
                    // Handshake failed
                    char buf[128];
                    mbedtls_strerror(res, buf, sizeof(buf));
                    Log::error(tag, Format("mbedtls_ssl_handshake failed: {1}", Str(buf)));
                    this->stop();
                }
            }

            template<typename Packet>
            void SecureSocket<Packet>::writable()
            {
                auto res = mbedtls_ssl_handshake(secure_context.get_context());

                if (res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE)
                {
                    // Handshake not yet complete
                }
                else if (res == 0)
                {
                    // Handshake is complete, write application data.
                    Socket<Packet>::writable();
                }
                else
                {
                    // Handshake failed
                    char buf[128];
                    mbedtls_strerror(res, buf, sizeof(buf));
                    Log::error(tag, Format("mbedtls_ssl_handshake failed: {1}", Str(buf)));
                    this->stop();
                }
            }

            template<typename Packet>
            void SecureSocket<Packet>::read_data()
            {
                // How much data to assemble the current packet?
                int wanted_length = this->rx_buffer.amount_wanted();


                int read_amount;
                do
                {
                    read_amount = mbedtls_ssl_read(secure_context.get_context(),
                                                   this->rx_buffer.get_write_pos(),
                                                   static_cast<size_t>(wanted_length));

                    if (read_amount > 0)
                    {
                        this->rx_buffer.data_received(static_cast<int>(read_amount));
                        if (this->rx_buffer.is_error())
                        {
                            Log::error(tag, "Assembly error");
                            this->stop();
                        }
                        else if (this->rx_buffer.is_packet_complete())
                        {
                            DataAvailableEvent<Packet> d(&this->rx_buffer);
                            this->data_available.push(d);
                            this->rx_buffer.prepare_new_packet();
                        }
                    }

                }
                while (read_amount > 0 || read_amount == MBEDTLS_ERR_SSL_WANT_READ);

                if (read_amount < 0)
                {
                    char buf[128];
                    mbedtls_strerror(read_amount, buf, sizeof(buf));
                    Log::error(tag, Format("mbedtls_ssl_read failed: {1}", Str(buf)));
                    this->stop();
                }
            }

            template<typename Packet>
            void SecureSocket<Packet>::write_data()
            {
                int amount_sent = 0;

                do
                {
                    auto data_to_send = this->tx_buffer.get_data_to_send();
                    auto length = this->tx_buffer.get_remaining_data_length();

                    amount_sent = mbedtls_ssl_write(secure_context.get_context(),
                                                    data_to_send,
                                                    static_cast<size_t>(length));

                    if (amount_sent > 0)
                    {
                        this->tx_buffer.data_has_been_sent(amount_sent);

                        // Was a complete packet sent?
                        if (this->tx_buffer.is_in_progress())
                        {
                            this->elapsed_send_time.start();
                        }
                        else
                        {
                            // Let the application know it may now send another packet.
                            smooth::core::network::TransmitBufferEmptyEvent event(this->shared_from_this());
                            this->tx_empty.push(event);
                        }
                    }
                }
                while (amount_sent > 0
                       || amount_sent == MBEDTLS_ERR_SSL_WANT_READ
                       || amount_sent == MBEDTLS_ERR_SSL_WANT_WRITE);

                if (amount_sent < 0)
                {
                    char buf[128];
                    mbedtls_strerror(amount_sent, buf, sizeof(buf));
                    Log::error(tag, Format("mbedtls_ssl_write failed: {1}", Str(buf)));
                    this->stop();
                }
            }
        }
    }
}
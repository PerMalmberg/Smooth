//
// Created by permal on 7/10/17.
//

#pragma once

#include "Socket.h"
#include <openssl/ssl.h>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            /// An specialization of a Socket with SSL-capabilities.
            /// \tparam Packet The type of packet
            template<typename Packet>
            class SSLSocket
                    : public Socket<Packet>
            {
                public:
                    /// Creates an SSL socket for network communication, with the specified packet type.
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
                    create(IPacketSendBuffer<Packet>& tx_buffer,
                           IPacketReceiveBuffer<Packet>& rx_buffer,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status);

                    ~SSLSocket();
                    SSLSocket(IPacketSendBuffer<Packet>& tx_buffer, IPacketReceiveBuffer<Packet>& rx_buffer,
                              smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                              smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                              smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status);
                protected:

                    bool create_socket() override;
                    bool prepare_connected_socket() override;

                    void read_data(uint8_t* target, int max_length) override;
                    void write_data(const uint8_t* src, int length) override;

                    SSL_CTX* ctx;
                    SSL* ssl;
            };

            template<typename Packet>
            std::shared_ptr<ISocket> SSLSocket<Packet>::create(IPacketSendBuffer<Packet>& tx_buffer,
                                                          IPacketReceiveBuffer<Packet>& rx_buffer,
                                                          smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                                          smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                                          smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status)
            {

                // This class is solely used to enabled access to the protected Socket<Packet> constructor from std::make_shared<>
                class MakeSharedActivator
                        : public SSLSocket<Packet>
                {
                    public:
                        MakeSharedActivator(IPacketSendBuffer<Packet>& tx_buffer,
                                            IPacketReceiveBuffer<Packet>& rx_buffer,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status)
                                : SSLSocket<Packet>(tx_buffer, rx_buffer, tx_empty, data_available, connection_status)
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

            template<typename Packet>
            SSLSocket<Packet>::SSLSocket(IPacketSendBuffer<Packet>& tx_buffer, IPacketReceiveBuffer<Packet>& rx_buffer,
                                    smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                    smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<Packet>>& data_available,
                                    smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status
            )
                    : Socket<Packet>(tx_buffer, rx_buffer, tx_empty, data_available, connection_status),
                      ctx(nullptr), ssl(nullptr)
            {
            }

            template<typename Packet>
            SSLSocket<Packet>::~SSLSocket()
            {
                if (ssl)
                {
                    SSL_free(ssl);
                }

                if (ctx)
                {
                    SSL_CTX_free(ctx);
                }
            }

            template<typename Packet>
            bool SSLSocket<Packet>::create_socket()
            {
                if (ctx != nullptr)
                {
                    SSL_CTX_free(ctx);
                }

                bool res = Socket<Packet>::create_socket();

                if (res)
                {
                    ctx = SSL_CTX_new(TLSv1_2_client_method());
                }

                return ctx != nullptr;
            }

            template<typename Packet>
            bool SSLSocket<Packet>::prepare_connected_socket()
            {
                int res = -1;

                if (ssl != nullptr)
                {
                    SSL_free(ssl);
                }

                ssl = SSL_new(ctx);
                if (ssl != nullptr)
                {
                    SSL_set_fd(ssl, this->get_socket_id());
                }

                if (ssl != nullptr)
                {
                    ESP_LOGV("SSLSocket", "Connecting...")
                    res = SSL_connect(ssl);
                    ESP_LOGV("SSLSocket", "...done")
                    if (res != 1)
                    {
                        ESP_LOGE("SSLSocket", "Connection failed: %d", res);
                    }
                }

                return ssl != nullptr && res == 1;
            }

            template<typename Packet>
            void SSLSocket<Packet>::read_data(uint8_t* target, int max_length)
            {

                bool read_blocked = false;

                do
                {
                    int read_count = SSL_read(ssl, target, max_length);

                    if (read_count > 0)
                    {
                        this->rx_buffer.data_received(read_count);
                        if (this->rx_buffer.is_error())
                        {
                            ESP_LOGE("Socket", "Assembly error");
                            this->stop();
                        }
                        else if (this->rx_buffer.is_packet_complete())
                        {
                            DataAvailableEvent<Packet> d(&this->rx_buffer);
                            this->data_available.push(d);
                            this->rx_buffer.prepare_new_packet();
                        }
                    }
                    else
                    {
                        int err = SSL_get_error(ssl, read_count);
                        if (err == SSL_ERROR_NONE)
                        {
                            // No error, ignore.
                        }
                        else if (err == SSL_ERROR_WANT_WRITE)
                        {
                            ESP_LOGV("SSLSocket", "SSL_ERROR_WANT_WRITE");
                            read_blocked = true;
                        }
                        else if (err == SSL_ERROR_ZERO_RETURN)
                        {
                            ESP_LOGV("SSLSocket", "SSL_ERROR_ZERO_RETURN");
                            this->stop();
                        }
                        else if (err == SSL_ERROR_WANT_READ)
                        {
                            // No data available right now
                            ESP_LOGV("SSLSocket", "SSL_ERROR_WANT_READ");
                            read_blocked = true;
                        }
                        else
                        {
                            // Unknown error
                            ESP_LOGV("SSLSocket", "Error: %d", err);
                            this->stop();
                        }
                    }
                }
                while (SSL_pending(ssl) && !read_blocked);
            }

            template<typename Packet>
            void SSLSocket<Packet>::write_data(const uint8_t* src, int length)
            {
                int amount_sent = SSL_write(ssl, src, length);

                if (amount_sent > 0)
                {
                    this->tx_buffer.data_has_been_sent(amount_sent);

                    // Was a complete packet sent?
                    if (!this->tx_buffer.is_in_progress())
                    {
                        // Let the application know it may now send another packet.
                        smooth::core::network::TransmitBufferEmptyEvent event(this->shared_from_this());
                        this->tx_empty.push(event);
                    }
                }
                else
                {
                    int err = SSL_get_error(ssl, amount_sent);
                    if (err == SSL_ERROR_NONE)
                    {
                        // No error, ignore.
                    }
                    else if (err == SSL_ERROR_WANT_WRITE)
                    {
                        ESP_LOGV("SSLSocket", "SSL_ERROR_WANT_WRITE");
                    }
                    else if (err == SSL_ERROR_ZERO_RETURN)
                    {
                        ESP_LOGV("SSLSocket", "SSL_ERROR_ZERO_RETURN");
                        this->stop();
                    }
                    else if (err == SSL_ERROR_WANT_READ)
                    {
                        // No data available right now
                        ESP_LOGV("SSLSocket", "SSL_ERROR_WANT_READ");
                    }
                    else
                    {
                        // Unknown error
                        ESP_LOGV("SSLSocket", "Error: %d", err);
                        this->stop();
                    }
                }
            }
        }
    }
}
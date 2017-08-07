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
            template<typename T>
            class SSLSocket
                    : public Socket<T>
            {
                public:
                    static std::shared_ptr<ISocket>
                    create(IPacketSendBuffer<T>& tx_buffer,
                           IPacketReceiveBuffer<T>& rx_buffer,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<T>>& data_available,
                           smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status);

                    ~SSLSocket();
                    SSLSocket(IPacketSendBuffer<T>& tx_buffer, IPacketReceiveBuffer<T>& rx_buffer,
                              smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                              smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<T>>& data_available,
                              smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status);
                protected:

                    bool create_socket() override;
                    bool prepare_connected_socket() override;

                    void read_data(uint8_t* target, int max_length) override;
                    void write_data(const uint8_t* src, int length) override;

                    SSL_CTX* ctx;
                    SSL* ssl;
            };

            template<typename T>
            std::shared_ptr<ISocket> SSLSocket<T>::create(IPacketSendBuffer<T>& tx_buffer,
                                                          IPacketReceiveBuffer<T>& rx_buffer,
                                                          smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                                          smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<T>>& data_available,
                                                          smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status)
            {

                // This class is solely used to enabled access to the protected Socket<T> constructor from std::make_shared<>
                class MakeSharedActivator
                        : public SSLSocket<T>
                {
                    public:
                        MakeSharedActivator(IPacketSendBuffer<T>& tx_buffer,
                                            IPacketReceiveBuffer<T>& rx_buffer,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<T>>& data_available,
                                            smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status)
                                : SSLSocket<T>(tx_buffer, rx_buffer, tx_empty, data_available, connection_status)
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
            SSLSocket<T>::SSLSocket(IPacketSendBuffer<T>& tx_buffer, IPacketReceiveBuffer<T>& rx_buffer,
                                    smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent>& tx_empty,
                                    smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<T>>& data_available,
                                    smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent>& connection_status
            )
                    : Socket<T>(tx_buffer, rx_buffer, tx_empty, data_available, connection_status),
                      ctx(nullptr), ssl(nullptr)
            {
            }

            template<typename T>
            SSLSocket<T>::~SSLSocket()
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

            template<typename T>
            bool SSLSocket<T>::create_socket()
            {
                if (ctx != nullptr)
                {
                    SSL_CTX_free(ctx);
                }

                bool res = Socket<T>::create_socket();

                if (res)
                {
                    ctx = SSL_CTX_new(TLSv1_2_client_method());
                }

                return ctx != nullptr;
            }

            template<typename T>
            bool SSLSocket<T>::prepare_connected_socket()
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

            template<typename T>
            void SSLSocket<T>::read_data(uint8_t* target, int max_length)
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
                            DataAvailableEvent<T> d(&this->rx_buffer);
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

            template<typename T>
            void SSLSocket<T>::write_data(const uint8_t* src, int length)
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
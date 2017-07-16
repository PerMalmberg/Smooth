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
#include <smooth/util/CircularBuffer.h>
#include <smooth/ipc/TaskEventQueue.h>
#include <smooth/network/TransmitBufferEmptyEvent.h>
#include <smooth/network/DataAvailableEvent.h>
#include <smooth/network/PacketSendBuffer.h>
#include <smooth/network/SocketDispatcher.h>
#include <smooth/network/ConnectionStatusEvent.h>

namespace smooth
{
    namespace network
    {

        template<typename T>
        class Socket
                : public ISocket
        {
            public:
                friend class smooth::network::SocketDispatcher;

                Socket(IPacketSendBuffer<T>& tx_buffer, IPacketReceiveBuffer<T>& rx_buffer,
                       smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmptyEvent>& tx_empty,
                       smooth::ipc::TaskEventQueue<smooth::network::DataAvailableEvent<T>>& data_available,
                       smooth::ipc::TaskEventQueue<smooth::network::ConnectionStatusEvent>& connection_status);

                virtual ~Socket()
                {
                }

                bool start(std::shared_ptr<InetAddress> ip);
                bool restart();
                void stop() override;

                void readable() override;

                void writable() override;

            protected:
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
                smooth::ipc::TaskEventQueue<smooth::network::DataAvailableEvent<T>>& data_available;
                smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmptyEvent>& tx_empty;

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
                void check_if_connection_is_completed() override;
                int socket_id = -1;
                std::shared_ptr<InetAddress> ip;
                bool started = false;
                bool connected = false;
                smooth::ipc::TaskEventQueue<smooth::network::ConnectionStatusEvent>& connection_status;
        };

        template<typename T>
        Socket<T>::Socket(IPacketSendBuffer<T>& tx_buffer, IPacketReceiveBuffer<T>& rx_buffer,
                          smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmptyEvent>& tx_empty,
                          smooth::ipc::TaskEventQueue<smooth::network::DataAvailableEvent<T>>& data_available,
                          smooth::ipc::TaskEventQueue<smooth::network::ConnectionStatusEvent>& connection_status
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
            this->ip = ip;
            bool res = ip->is_valid() && create_socket();
            if (res)
            {
                SocketDispatcher::instance().add_socket(this);
            }

            return res;
        }

        template<typename T>
        bool Socket<T>::restart()
        {
            bool res = ip && ip->is_valid() && create_socket();
            if (res)
            {
                SocketDispatcher::instance().add_socket(this);
            }

            return res;
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
                    ESP_LOGE("Socket", "Creation failed:: %s", strerror(errno));
                }
                else
                {
                    res = set_non_blocking();
                    int no_delay = 1;
                    res &= setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(no_delay)) == 0;
                    if (res)
                    {
                        ESP_LOGV("Socket", "Created: id: %d", socket_id);
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
                ESP_LOGE("Socket", "Could not get socket flags: %s", strerror(errno));
                res = false;
            }
            else if (fcntl(socket_id, F_SETFL, opts | O_NONBLOCK) < 0)
            {
                ESP_LOGE("Socket", "Could not set socket flags: %s", strerror(errno));
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
                    const char* error = strerror(errno);
                    ESP_LOGV("Socket", "Error: %d: %s", socket_id, error);
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
                smooth::network::TransmitBufferEmptyEvent msg(this);
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
                    const char* error = strerror(errno);
                    ESP_LOGV("Socket", "error: readable(): %d: %s", socket_id, error);
                    stop();
                }
            }
            else
            {
                rx_buffer.data_received(read_count);
                if (rx_buffer.is_error())
                {
                    ESP_LOGE("Socket", "Assembly error");
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
            int amount_sent = send(socket_id, tx_buffer.get_data_to_send(), tx_buffer.get_remaining_data_length(), 0);

            if (amount_sent == -1)
            {
                const char* error = strerror(errno);
                ESP_LOGE("Socket", "Failed during send: %d, %s", errno, error);
                stop();
            }
            else
            {
                tx_buffer.data_has_been_sent(amount_sent);

                // Was a complete packet sent?
                if (!tx_buffer.is_in_progress())
                {
                    // Let the application know it may now send another packet.
                    smooth::network::TransmitBufferEmptyEvent msg(this);
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
                    int res = connect(socket_id, ip->get_socket_address(), ip->get_socket_address_length());
                    if (res == 0 || (res == -1 && errno == EINPROGRESS))
                    {
                        ESP_LOGV("Socket", "started: %d", socket_id);
                        started = true;
                    }
                    else
                    {
                        const char* error = strerror(errno);
                        ESP_LOGV("Socket", "connect: %d, errno: %s", res, error);
                    }
                }

                if (!started)
                {
                    stop();
                }
            }
        }

        template<typename T>
        void Socket<T>::check_if_connection_is_completed()
        {
            int res = connect(socket_id, ip->get_socket_address(), ip->get_socket_address_length());
            if (res == -1 && errno == EISCONN)
            {
                if (prepare_connected_socket())
                {
                    connected = true;
                    ESP_LOGV("Socket", "Connected %d", socket_id);
                    connection_status.push(ConnectionStatusEvent(this, true));
                }
                else
                {
                    stop();
                }
            }
        }

        template<typename T>
        void Socket<T>::stop()
        {
            shutdown(socket_id, SHUT_RDWR);
            close(socket_id);
            started = false;
            connected = false;
            tx_buffer.clear();
            rx_buffer.clear();
            SocketDispatcher::instance().socket_closed(this);

            // Reset socket_id last as it is used as an identifier up to this point.
            socket_id = -1;

            connection_status.push(ConnectionStatusEvent(this, false));
        }
    }
}

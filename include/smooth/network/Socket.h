//
// Created by permal on 6/25/17.
//

#pragma once

#include <cstring>
#include <sys/socket.h>

#undef bind

#include <array>
#include "InetAddress.h"
#include <memory>
#include "ISocket.h"
#include <smooth/util/CircularBuffer.h>
#include <smooth/ipc/TaskEventQueue.h>
#include <smooth/network/TransmitBufferEmpty.h>
#include <smooth/network/DataAvailable.h>
#include <smooth/network/PacketBuffer.h>
#include <smooth/network/SocketDispatcher.h>

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

                Socket(IPacketBuffer<T>& tx_buffer, util::ICircularBuffer<char>& rx_buffer,
                       smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmpty>& tx_empty,
                       smooth::ipc::TaskEventQueue<smooth::network::DataAvailable>& data_available);

                virtual ~Socket()
                {
                }

                bool start(std::shared_ptr<InetAddress> ip);
                void stop() override;

                void readable() override;

                void writable() override;

            private:
                void internal_start() override;

                bool is_started() override;

                bool is_connected() override
                {
                    return connected;
                }

                int get_socket_id() override
                {
                    return socket_id;
                }

                bool set_non_blocking();

                bool has_data_to_transmit() override
                {
                    // Also check on connected state so that we don't try to send data
                    // when the socket just has been closed while in SocketDispatcher::tick()
                    return connected && !tx_buffer.is_empty();
                }

                void check_if_connection_is_completed() override;

                bool create_socket();

                int socket_id = -1;
                std::shared_ptr<InetAddress> ip;
                bool started = false;
                bool connected = false;
                IPacketBuffer<T>& tx_buffer;
                smooth::util::ICircularBuffer<char>& rx_buffer;
                smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmpty>& tx_empty;
                smooth::ipc::TaskEventQueue<smooth::network::DataAvailable>& data_available;
        };

        template<typename T>
        Socket<T>::Socket(IPacketBuffer<T>& tx_buffer, util::ICircularBuffer<char>& rx_buffer,
                          smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmpty>& tx_empty,
                          smooth::ipc::TaskEventQueue<smooth::network::DataAvailable>& data_available)
                :
                tx_buffer(tx_buffer), rx_buffer(rx_buffer), tx_empty(tx_empty), data_available(data_available)
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
        bool Socket<T>::create_socket()
        {
            bool res = false;

            if (socket_id < 0)
            {
                socket_id = socket(ip->get_protocol_family(), SOCK_STREAM, 0);

                if (socket_id == -1)
                {
                    ESP_LOGV("Socket", "Creation failed, id: %s", strerror(errno));
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
                ESP_LOGV("Socket", "readable");
                // attempt to read available data
                // Any room left in buffer?
                if (rx_buffer.available_slots() > 0)
                {
                    char data;
                    int read_count = 0;

                    do
                    {
                        read_count = recv(socket_id, &data, sizeof(data), 0);
                        ESP_LOGV("read", "%d", read_count);
                        if (read_count > 0)
                        {
                            rx_buffer.put(data);
                        }
                    }
                    while (rx_buffer.available_slots() > 0 && read_count > 0);

                    if (read_count == -1)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            const char* error = strerror(errno);
                            ESP_LOGV("Socket", "readable(): %d: %s", socket_id, error);
                            stop();
                        }
                    }

                    if (connected)
                    {
                        DataAvailable d(&rx_buffer);
                        data_available.push(d);
                    }
                }

            }
        }

        template<typename T>
        void Socket<T>::writable()
        {
            // Any data to send?
            if(tx_buffer.is_empty())
            {
                // Let the application know it may send a packet.
                smooth::network::TransmitBufferEmpty msg(this);
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
                    // Try to send as much as possible up to CONFIG_TCP_SND_BUF_DEFAULT. This should avoid ever getting
                    // an EWOULDBLOCK or EAGAIN error since the socket is prepared to accept that amount of data.
                    int amount_sent = send(socket_id, tx_buffer.get_data_to_send(),
                                           tx_buffer.get_remaining_data_length(CONFIG_TCP_SND_BUF_DEFAULT), 0);

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
                            smooth::network::TransmitBufferEmpty msg(this);
                            tx_empty.push(msg);
                        }
                    }
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
                connected = true;
                ESP_LOGV("Socket", "Connected %d", socket_id);
                // QQQ Notify receiver connection is completed
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
        }
    }
}

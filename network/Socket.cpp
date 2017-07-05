//
// Created by permal on 6/25/17.
//

#include <cstring>
#include <smooth/network/Socket.h>

#include <esp_log.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <smooth/network/SocketDispatcher.h>

namespace smooth
{
    namespace network
    {
        Socket::Socket(ITransferBuffer& tx_buffer, ITransferBuffer& rx_buffer)
                : tx_buffer(tx_buffer), rx_buffer(rx_buffer)
        {
        }

        bool Socket::start(std::shared_ptr<InetAddress> ip)
        {
            this->ip = ip;
            bool res = ip->is_valid() && create_socket();
            if (res)
            {
                SocketDispatcher::instance().add_socket(this);
            }

            return res;
        }

        bool Socket::create_socket()
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
            else {
                res = true;
            }

            return res;
        }

        bool Socket::set_non_blocking()
        {
            bool res = true;

            auto opts = fcntl(socket_id, F_GETFL, 0);
            if (opts < 0)
            {
                ESP_LOGE("SocketWorker", "Could not get socket flags: %s", strerror(errno));
                res = false;
            }
            else if (fcntl(socket_id, F_SETFL, opts | O_NONBLOCK) < 0)
            {
                ESP_LOGE("SocketWorker", "Could not set socket flags: %s", strerror(errno));
                res = false;
            }

            return res;
        }

        void Socket::readable()
        {
            // Detect disconnection
            char b[1];
            int res = recv(socket_id, b, 1, MSG_PEEK);
            if (res <= 0  )
            {
                if( res == -1 )
                {
                    const char* error = strerror(errno);
                    ESP_LOGV("Socket", "Disconnected: %d: %s", socket_id, error);
                }

                stop();
            }
            else
            {
                //rx_buffer.space_left()
            }
        }

        void Socket::writable()
        {
            if (!connected)
            {
                int result;
                socklen_t size = sizeof(result);
                if (getsockopt(socket_id, SOL_SOCKET, SO_ERROR, &result, &size) < 0)
                {
                    ESP_LOGV("Socket", "getsockopt failed");
                    stop();
                }
                else if (result != 0)
                {
                    ESP_LOGV("Socket", "getsockopt: %d", result);
                    stop();
                }
                else
                {
                    ESP_LOGV("Socket", "Now connected: %d", socket_id);
                    connected = true;
                }
            }
            else if (has_data_to_transmit())
            {
                int res = ::send(socket_id, tx_buffer.data(), tx_buffer.size(), 0);
                if (res == -1)
                {
                    const char* error = strerror(errno);
                    ESP_LOGE("Socket", "Failed during send: %s", error);
                    stop();
                }
                else
                {
                    tx_buffer.take(res);
                    // QQQ: Notify sender that data has been sent.
                }
            }
        }


        bool Socket::is_started()
        {
            return started;
        }

        void Socket::internal_start()
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

        void Socket::stop()
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

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
        Socket::Socket(IDataAvailable& destination)
                : destination(destination)
        {
        }

        bool Socket::start(std::shared_ptr<InetAddress> ip)
        {
            this->ip = ip;
            bool res = ip->is_valid();
            if (res)
            {
                SocketDispatcher::instance().add_socket(this);
            }

            return res;
        }

        bool Socket::create_socket()
        {
            bool res = false;
            socket_id = socket(ip->get_protocol_family(), SOCK_STREAM, 0);

            if (socket_id == -1)
            {
                ESP_LOGV("Socket", "Creation failed, id: %s", strerror(errno));
            }
            else
            {
                res = set_non_blocking();
                int no_delay = 1;
                setsockopt(socket_id, IPPROTO_TCP, TCP_NODELAY, &no_delay, sizeof(no_delay));
                if (res)
                {
                    ESP_LOGV("Socket", "Created: id: %d", socket_id);
                }
            }

            return res;
        }

        void Socket::transmit(std::shared_ptr<ISendBuffer> buffer)
        {
            send_buffer = buffer;
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
            if (res == 0 || (res == -1 && errno == EWOULDBLOCK))
            {
                ESP_LOGV("Socket", "Disconnected: %d", socket_id);
                stop();
            }
            else
            {

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
            else if (send_buffer)
            {
                int res = ::send(socket_id, send_buffer->data(), send_buffer->size(), 0);
                if (res == -1)
                {
                    const char* error = strerror(errno);
                    ESP_LOGE("Socket", "Failed during send: %s", error);
                    stop();
                    send_buffer.reset();
                }
                else
                {
                    send_buffer->take(res);
                    if (send_buffer->size() == 0)
                    {
                        // All data sent
                        send_buffer.reset();
                    }
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
                bool could_create = true;
                if (socket_id < 0)
                {
                    could_create = create_socket();
                }

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
            SocketDispatcher::instance().socket_closed(this);
            socket_id = -1;
        }

    }
}

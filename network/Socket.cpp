//
// Created by permal on 6/25/17.
//

#include <cstring>
#include <smooth/network/Socket.h>

#include <esp_log.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

namespace smooth
{
    namespace network
    {
        bool Socket::start(InetAddress& ip)
        {
            bool res = ip.is_valid();
            if (res)
            {
                socket_id = socket(ip.get_protocol_family(), SOCK_STREAM, 0);
                ESP_LOGV("Socket", "socket_id: %d", socket_id);

                int res = connect(socket_id, ip.get_socket_address(), ip.get_socket_address_length());
                const char* error = strerror(errno);
                ESP_LOGV("Socket", "connect: %d, errno: %s", res, error);
            }

            return res;
        }

        void Socket::stop()
        {
            shutdown(socket_id, SHUT_RDWR);
            close(socket_id);
        }

    }
}

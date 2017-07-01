//
// Created by permal on 7/1/17.
//

#include <smooth/network/IPv4.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <esp_log.h>

namespace smooth
{
    namespace network
    {

        IPv4::IPv4(const std::string& address, uint16_t port) : InetAddress()
        {
            memset(&sock_address, 0, sizeof(sock_address));
            sock_address.sin_family = AF_INET;
            sock_address.sin_port = htons(port);
            valid = inet_pton(AF_INET, address.c_str(), &sock_address.sin_addr) == 1;
        }

        sockaddr* IPv4::get_socket_address()
        {
            return reinterpret_cast<sockaddr*>(&sock_address);
        }

        socklen_t IPv4::get_socket_address_length() const
        {
            return sizeof(sock_address);
        }
    };
}

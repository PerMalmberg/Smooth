//
// Created by permal on 7/1/17.
//

#include <smooth/core/network/IPv6.h>
#include <arpa/inet.h>

namespace smooth
{
    namespace  core
    {
        namespace network
        {

            IPv6::IPv6(const std::string& ip_number_as_string, uint16_t port) : InetAddress(ip_number_as_string, port)
            {
                memset(&sock_address, 0, sizeof(sock_address));
                sock_address.sin6_family = AF_INET6;
                sock_address.sin6_port = htons(port);
                valid = inet_pton(AF_INET, ip_number_as_string.c_str(), &sock_address.sin6_addr) == 1;
            }

            sockaddr* IPv6::get_socket_address()
            {
                return reinterpret_cast<sockaddr*>(&sock_address);
            }

            socklen_t IPv6::get_socket_address_length() const
            {
                return sizeof(sock_address);
            }
        };
    }
}

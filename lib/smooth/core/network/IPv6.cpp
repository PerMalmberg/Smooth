/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "smooth/core/network/IPv6.h"
#include <arpa/inet.h>

namespace smooth::core::network
{
    IPv6::IPv6(const std::string& ip_number_as_string, uint16_t port)
            : InetAddress(ip_number_as_string, port)
    {
        memset(&sock_address, 0, sizeof(sock_address));
        sock_address.sin6_family = AF_INET6;
        sock_address.sin6_port = htons(port);
        valid = inet_pton(AF_INET, ip_number_as_string.c_str(), &sock_address.sin6_addr) == 1;
    }

    IPv6::IPv6(const sockaddr_in6& address)
            : InetAddress("", 0),
              sock_address(address)
    {
    }

    bool IPv6::resolve_ip()
    {
        return true;
    }

    sockaddr* IPv6::get_socket_address()
    {
        return reinterpret_cast<sockaddr*>(&sock_address);
    }

    socklen_t IPv6::get_socket_address_length() const
    {
        return sizeof(sock_address);
    }
}

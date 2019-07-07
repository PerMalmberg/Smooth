// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <smooth/core/network/IPv6.h>
#include <arpa/inet.h>

namespace smooth::core
    {
        namespace network
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
        };
    }

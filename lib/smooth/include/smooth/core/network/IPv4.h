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

#pragma once

#include "InetAddress.h"
#include <arpa/inet.h>
#include <regex>

namespace smooth::core::network
{
    /// Represents an IPv4 address and port number.
    class IPv4
        : public InetAddress
    {
        public:
            /// Constructor
            /// \param ip_number_as_string The IP number in dotted decimal format, e.g. 10.0.0.1
            /// \param port The port to connect to.
            IPv4(int octet_1, int octet_2, int octet_3, int octet_4, uint16_t port);

            IPv4(const std::string& hostname, uint16_t port);

            explicit IPv4(const sockaddr_in& addr);

            bool resolve_ip() override;

            sockaddr* get_socket_address() override;

            socklen_t get_socket_address_length() const override;

            int get_address_family() const override
            {
                return sock_address.sin_family;
            }

        private:
            sockaddr_in sock_address;
            static std::regex const numeric_ip;
    };
}

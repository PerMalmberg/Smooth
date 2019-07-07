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

#include <netinet/in.h>
#include "InetAddress.h"

namespace smooth::core::network
{
    /// Represents an IPv6 address and port number.
    /// Note: Untested.
    class IPv6
            : public InetAddress
    {
        public:
            /// Constructor
            /// \param ip_number_as_string The IP number in colon separated form,
            /// e.g. 2001:0db8:85a3:0000:0000:8a2e:0370:7334
            /// \param port The port to connect to.
            IPv6(const std::string& ip_number_as_string, uint16_t port);

            explicit IPv6(const sockaddr_in6& address);

            sockaddr* get_socket_address() override;

            socklen_t get_socket_address_length() const override;

            int get_address_family() const override
            {
                return sock_address.sin6_family;
            };

            bool resolve_ip() override;

        private:
            sockaddr_in6 sock_address;
    };
}
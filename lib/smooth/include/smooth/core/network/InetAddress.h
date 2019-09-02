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

#include <sys/socket.h>
#include <string>
#include <cstring>

namespace smooth::core::network
{
    /// Base class for network addresses and ports.
    class InetAddress
    {
        public:
            /// Constructor
            /// \param ip_as_string The IP in string form.
            /// \param port The port.
            InetAddress(std::string host, int port)
                    : host(std::move(host)), port(port)
            {
            }

            virtual ~InetAddress() = default;

            // Performs name resolution
            virtual bool resolve_ip() = 0;

            /// Gets the address family, e.g. AF_INET or AF_INET6
            /// \return The adress family
            virtual int get_address_family() const = 0;

            /// Gets the protocol family, e.g. PF_INET or PF_INET6
            /// \return Protocol family
            int get_protocol_family() const
            {
                return get_address_family() == AF_INET ? PF_INET : PF_INET6;
            }

            /// Gets the socket address
            /// \return Socket address
            virtual sockaddr* get_socket_address() = 0;

            /// Gets the length of the socket address
            /// \return Socket address length
            virtual socklen_t get_socket_address_length() const = 0;

            /// Gets the port
            /// \return The port
            int get_port() const
            {
                return port;
            }

            /// Gets the host
            /// \return The host
            std::string get_host() const
            {
                return host;
            }

            std::string get_resolved_ip() const
            {
                return resolved_ip;
            }

            /// Returns a value indicating if the IP address is valid.
            /// \return true or false
            bool is_valid() const
            {
                return valid;
            }

        protected:
            bool valid = false;
            std::string host;
            std::string resolved_ip;
            int port;
    };
}

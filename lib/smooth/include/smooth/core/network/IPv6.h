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

#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <netinet/in.h>
#pragma GCC diagnostic pop

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
            }

            bool resolve_ip() override;

        private:
            sockaddr_in6 sock_address;
    };
}

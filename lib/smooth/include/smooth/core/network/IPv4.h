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

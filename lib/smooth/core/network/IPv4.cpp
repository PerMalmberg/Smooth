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

#include <cstdint>
#include <sys/types.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <sys/socket.h>
#pragma GCC diagnostic pop
#include <netdb.h>
#include <arpa/inet.h>
#include "smooth/core/logging/log.h"
#include "smooth/core/network/IPv4.h"

using namespace smooth::core::logging;

namespace smooth::core::network
{
    std::regex const IPv4::numeric_ip{ R"(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})", std::regex::ECMAScript };

    IPv4::IPv4(int octet_1, int octet_2, int octet_3, int octet_4, uint16_t port)
            : InetAddress(
                  std::to_string(octet_1)
                  + "."
                  + std::to_string(octet_2)
                  + "."
                  + std::to_string(octet_3)
                  + "."
                  + std::to_string(octet_4),
                  port)
    {
        memset(&sock_address, 0, sizeof(sock_address));
    }

    IPv4::IPv4(const std::string& hostname, uint16_t port)
            : InetAddress(hostname, port)
    {
        memset(&sock_address, 0, sizeof(sock_address));
    }

    IPv4::IPv4(const sockaddr_in& addr)
            : InetAddress("", 0),
              sock_address(addr)
    {
    }

    sockaddr* IPv4::get_socket_address()
    {
        return reinterpret_cast<sockaddr*>(&sock_address);
    }

    socklen_t IPv4::get_socket_address_length() const
    {
        return sizeof(sock_address);
    }

    bool IPv4::resolve_ip()
    {
        std::smatch match;

        if (std::regex_match(host, match, numeric_ip))
        {
            // Already an IP
            resolved_ip = host;
            sock_address.sin_family = AF_INET;
            sock_address.sin_port = htons(static_cast<uint16_t>(port));
            valid = inet_pton(AF_INET, host.c_str(), &sock_address.sin_addr) == 1;
        }
        else
        {
            memset(&sock_address, 0, sizeof(sock_address));

            struct addrinfo* result = nullptr;

            auto res = getaddrinfo(host.c_str(), nullptr, nullptr, &result);

            if (res != 0)
            {
#if ESP_PLATFORM
                Log::error("IPv4", "Failed to lookup hostname {}", res);
#else
                Log::error("IPv4", "Failed to lookup hostname {}", gai_strerror(res));
#endif
            }
            else
            {
                // Pick the first match (there may be more than one)
                if (result->ai_family == AF_INET)
                {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
                    auto p = reinterpret_cast<sockaddr_in*>(result->ai_addr);
#pragma GCC diagnostic pop
                    sock_address = *p;
                    sock_address.sin_port = htons(static_cast<uint16_t>(port));

                    char address[100];
                    valid = inet_ntop(result->ai_family, &p->sin_addr, address, sizeof(address));

                    resolved_ip = address;

                    Log::info("IPv4", "{} resolved to {}", host, get_resolved_ip());
                }

                freeaddrinfo(result);
            }
        }

        return valid;
    }
}

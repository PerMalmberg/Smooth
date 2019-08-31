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

#include <cstdint>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/IPv4.h>

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
                Log::error("IPv4", Format("Failed to lookup hostname {1}", Int32(res)));
#else
                Log::error("IPv4", Format("Failed to lookup hostname {1}", Str(gai_strerror(res))));
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

                    Log::info("IPv4", Format("{1} resolved to {2}", Str(host), Str(get_resolved_ip())));
                }

                freeaddrinfo(result);
            }
        }

        return valid;
    }
}

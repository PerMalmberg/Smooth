//
// Created by permal on 7/1/17.
//

#include <cstdint>
#include <netdb.h>
#include <arpa/inet.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/network/IPv4.h>

using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        namespace network
        {

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
                sock_address.sin_family = AF_INET;
                sock_address.sin_port = htons(port);
                valid = inet_pton(AF_INET, ip_as_string.c_str(), &sock_address.sin_addr) == 1;
            }

            IPv4::IPv4(const std::string& hostname, uint16_t port)
                    : InetAddress("", port)
            {
                memset(&sock_address, 0, sizeof(sock_address));

                struct addrinfo *result;

                auto res = getaddrinfo(hostname.c_str(), nullptr, nullptr, &result);
                if(res != 0)
                {
                    Log::error("IPv4", Format("Failed to lookup hostname {1}", Str(gai_strerror(res))));
                }
                else
                {
                    // Pick the first match (there may be more than one)

                    if(result->ai_family == AF_INET)
                    {
                        auto p = reinterpret_cast<sockaddr_in*>(result->ai_addr);
                        sock_address = *p;
                        sock_address.sin_port = htons(port);

                        char address[100];
                        valid = inet_ntop(result->ai_family, &p->sin_addr, address, sizeof(address));

                        ip_as_string = address;

                        Log::info("IPv4", Format("{1}: {2}", Str(hostname), Str(ip_as_string)));
                    }

                    freeaddrinfo(result);
                }
            }

            sockaddr* IPv4::get_socket_address()
            {
                return reinterpret_cast<sockaddr*>(&sock_address);
            }

            socklen_t IPv4::get_socket_address_length() const
            {
                return sizeof(sock_address);
            }
        };
    }
}
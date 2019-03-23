//
// Created by permal on 7/1/17.
//

#pragma once

#include "InetAddress.h"
#include <arpa/inet.h>
#include <regex>

namespace smooth
{
    namespace core
    {
        namespace network
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

                    bool resolve_ip() override;
                    sockaddr* get_socket_address() override;
                    socklen_t get_socket_address_length() const override;

                    int get_address_family() const override
                    {
                        return sock_address.sin_family;
                    };

                private:
                    sockaddr_in sock_address;
                    static std::regex const numeric_ip;
            };
        }
    }
}
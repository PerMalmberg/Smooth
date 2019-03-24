//
// Created by permal on 7/1/17.
//

#pragma once

#include <netinet/in.h>
#include "InetAddress.h"

namespace smooth
{
    namespace core
    {
        namespace network
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
    }
}
//
// Created by permal on 7/1/17.
//

#pragma once

#include "InetAddress.h"

namespace smooth
{
    namespace core
    {
        namespace network
        {
            // Note: Untested.
            class IPv6
                    : public InetAddress
            {
                public:
                    IPv6(const std::string& address, uint16_t port);

                    sockaddr* get_socket_address() override;
                    socklen_t get_socket_address_length() const override;

                    int get_address_family() const
                    {
                        return sock_address.sin6_family;
                    };

                private:
                    sockaddr_in6 sock_address;
            };
        }
    }
}
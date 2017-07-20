//
// Created by permal on 7/1/17.
//

#pragma once

#include "InetAddress.h"
#include <arpa/inet.h>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            class IPv4
                    : public InetAddress
            {
                public:
                    IPv4(const std::string& address, uint16_t port);

                    sockaddr* get_socket_address() override;
                    socklen_t get_socket_address_length() const override;

                    int get_address_family() const
                    {
                        return sock_address.sin_family;
                    };

                private:
                    sockaddr_in sock_address;
            };
        }
    }
}
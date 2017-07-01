//
// Created by permal on 7/1/17.
//

#pragma once

#include <sys/socket.h>
#include <string>
#include <cstring>

namespace smooth
{
    namespace network
    {
        class InetAddress
        {
            public:
                InetAddress() = default;

                virtual int get_address_family() const = 0;

                int get_protocol_family() const
                {
                    return get_address_family() == AF_INET ? PF_INET : PF_INET6;
                }

                virtual sockaddr* get_socket_address() = 0;
                virtual socklen_t get_socket_address_length() const = 0;

                bool is_valid() const
                {
                    return valid;
                }

            protected:
                bool valid = false;
        };
    }
}

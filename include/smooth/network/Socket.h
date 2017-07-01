//
// Created by permal on 6/25/17.
//

#pragma once

#include <cstring>
#include <sys/socket.h>
#include "InetAddress.h"

namespace smooth
{
    namespace network
    {
        class Socket
        {
            public:
                Socket() = default;

                virtual ~Socket()
                {
                }

                bool start(InetAddress& ip);
                void stop();
            private:
                int socket_id;
        };
    }
}

//
// Created by permal on 7/9/17.
//

#pragma once

#include <smooth/network/ISocket.h>

namespace smooth
{
    namespace network
    {
        class ConnectionStatus
        {
            public:
                ConnectionStatus() = default;

                ConnectionStatus(smooth::network::ISocket* sock, bool is_connected)
                        : sock(sock),
                          connected(is_connected)
                {
                }

                bool is_connected() const
                {
                    return connected;
                }

                smooth::network::ISocket* get_socket()
                {
                    return sock;
                }

            private:
                smooth::network::ISocket* sock;
                bool connected;
        };
    }
}
//
// Created by permal on 7/9/17.
//

#pragma once

#include <smooth/core/network/ISocket.h>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            class ConnectionStatusEvent
            {
                public:
                    ConnectionStatusEvent() = default;

                    ConnectionStatusEvent(smooth::core::network::ISocket* sock, bool is_connected)
                            : sock(sock),
                              connected(is_connected)
                    {
                    }

                    bool is_connected() const
                    {
                        return connected;
                    }

                    smooth::core::network::ISocket* get_socket()
                    {
                        return sock;
                    }

                private:
                    smooth::core::network::ISocket* sock;
                    bool connected;
            };
        }
    }
}
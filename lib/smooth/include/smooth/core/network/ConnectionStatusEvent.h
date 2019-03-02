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
            /// Event sent when a socket is connected or disconnected.
            class ConnectionStatusEvent
            {
                public:
                    ConnectionStatusEvent() = default;
                    ConnectionStatusEvent(const ConnectionStatusEvent&) = default;

                    ConnectionStatusEvent(std::shared_ptr<smooth::core::network::ISocket>& socket, bool is_connected)
                            : sock(socket),
                              connected(is_connected)
                    {
                    }

                    bool is_connected() const
                    {
                        return connected;
                    }

                    const std::shared_ptr<ISocket>& get_socket() const
                    {
                        return sock;
                    }

                private:
                    std::shared_ptr<smooth::core::network::ISocket> sock;
                    bool connected = false;
            };
        }
    }
}
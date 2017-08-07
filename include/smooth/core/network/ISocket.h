//
// Created by permal on 7/2/17.
//

#pragma once

#undef bind
#include <memory>

#include "InetAddress.h"

namespace smooth
{
    namespace core
    {
        namespace network
        {
            class SocketDispatcher;

            /// Interface for sockets
            class ISocket
            {
                    friend class smooth::core::network::SocketDispatcher;

                public:
                    /// Initiates the connection to the provided IP. After this call events will arrive
                    /// via the response methods for data available, TX buffer empty, connection status etc.
                    /// \param ip The address to connect to (an instance of either IPv4 or IPv6).
                    /// \return true if the socket could be started and connection attempt initiated (but possibly not succeeded or yet completed)
                    virtual bool start(std::shared_ptr<InetAddress> ip) = 0;
                    /// Stops the socket, disconnected if currently connected.
                    virtual void stop() = 0;
                    /// Restarts the socket, disconnecting if currently connected.
                    /// \return true if the internal call to start() succeeds.
                    virtual bool restart() = 0;
                    /// Returns a value static if the socket is active (true) or not (false). Says nothing about if the
                    /// socket is connected or not (use the response method for ConnectionStatusEvent for that).
                    /// \return true or false, depending on status.
                    virtual bool is_active() = 0;
                protected:
                    virtual int get_socket_id() = 0;
                private:
                    virtual bool is_connected() = 0;
                    virtual void readable() = 0;
                    virtual void writable() = 0;
                    virtual bool has_data_to_transmit() = 0;
                    virtual void internal_start() = 0;
                    virtual bool check_if_connection_is_completed() = 0;
                    virtual void publish_connected_status(std::shared_ptr<ISocket>& socket) = 0;
            };
        }
    }
}
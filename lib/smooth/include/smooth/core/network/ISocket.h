//
// Created by permal on 7/2/17.
//

#pragma once

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
                    static const int INVALID_SOCKET = -1;

#ifdef ESP_PLATFORM
                    // lwip doesn't signal SIGPIPE
                    static const int SEND_FLAGS = 0;
#else
                    // Disable SIGPIPE during send()-calls.
                    static const int SEND_FLAGS = MSG_NOSIGNAL;
#endif

                    virtual ~ISocket() = default;

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

                    /// Returns true if the last send attempt has expired
                    /// \return true if the socket has not been able to send the data within the set limit.
                    virtual bool has_send_expired() const = 0;

                    /// Returns the socket id.
                    /// \return The socket id, possibly INVALID_SOCKET.
                    virtual int get_socket_id() = 0;
                protected:
                private:
                    virtual bool is_connected() = 0;
                    virtual void readable() = 0;
                    virtual void writable() = 0;
                    virtual bool has_data_to_transmit() = 0;
                    virtual bool internal_start() = 0;
                    virtual void publish_connected_status() = 0;
                    virtual void stop_internal() = 0;
                    virtual void clear_socket_id() = 0;
            };
        }
    }
}
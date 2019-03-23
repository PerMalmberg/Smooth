//
// Created by permal on 7/1/17.
//

#pragma once

#include <sys/socket.h>
#include <string>
#include <cstring>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            /// Base class for network addresses and ports.
            class InetAddress
            {
                public:
                    /// Constructor
                    /// \param ip_as_string The IP in string form.
                    /// \param port The port.
                    InetAddress(std::string host, int port)
                            : host(std::move(host)), port(port)
                    {
                    }

                    virtual ~InetAddress() = default;

                    // Performs name resolution
                    virtual bool resolve_ip() = 0;

                    /// Gets the address family, e.g. AF_INET or AF_INET6
                    /// \return The adress family
                    virtual int get_address_family() const = 0;

                    /// Gets the protocol family, e.g. PF_INET or PF_INET6
                    /// \return Protocol family
                    int get_protocol_family() const
                    {
                        return get_address_family() == AF_INET ? PF_INET : PF_INET6;
                    }

                    /// Gets the socket address
                    /// \return Socket address
                    virtual sockaddr* get_socket_address() = 0;

                    /// Gets the length of the socket address
                    /// \return Socket address length
                    virtual socklen_t get_socket_address_length() const = 0;

                    /// Gets the port
                    /// \return The port
                    int get_port() const
                    {
                        return port;
                    }

                    /// Gets the host
                    /// \return The host
                    std::string get_host() const
                    {
                        return host;
                    }

                    std::string get_resolved_ip() const
                    {
                        return resolved_ip;
                    }

                    /// Returns a value indicating if the IP address is valid.
                    /// \return true or false
                    bool is_valid() const
                    {
                        return valid;
                    }

                protected:
                    bool valid = false;
                    std::string host;
                    std::string resolved_ip;
                    int port;
            };
        }
    }
}
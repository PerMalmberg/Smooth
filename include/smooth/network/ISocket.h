//
// Created by permal on 7/2/17.
//

#pragma once

#undef bind
#include <memory>

#include "InetAddress.h"

namespace smooth
{
    namespace network
    {
        class SocketDispatcher;

        class ISocket
        {
                friend class smooth::network::SocketDispatcher;

            public:
                virtual void stop() = 0;
                virtual bool start(std::shared_ptr<InetAddress> ip) = 0;
                virtual bool restart() = 0;

            private:
                virtual bool is_connected() = 0;
                virtual void readable() = 0;
                virtual void writable() = 0;
                virtual bool has_data_to_transmit() = 0;
                virtual void internal_start() = 0;
                virtual bool is_started() = 0;
                virtual int get_socket_id() = 0;
                virtual void check_if_connection_is_completed() = 0;
        };
    }
}
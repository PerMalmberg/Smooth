//
// Created by permal on 6/25/17.
//

#pragma once

#include <cstring>
#include <sys/socket.h>

#undef bind

#include "InetAddress.h"
#include <memory>
#include "ISocket.h"
#include "TransferBuffer.h"

namespace smooth
{
    namespace network
    {
        class SocketDispatcher;

        class Socket
                : public ISocket
        {
            public:
                friend class smooth::network::SocketDispatcher;

                Socket(ITransferBuffer& tx_buffer, ITransferBuffer& rx_buffer);

                virtual ~Socket()
                {
                }

                bool start(std::shared_ptr<InetAddress> ip);
                void stop() override;

                void readable() override;

                void writable() override;

            private:
                void internal_start() override;

                bool is_started() override;

                int get_socket_id() override
                {
                    return socket_id;
                }

                bool set_non_blocking();

                bool has_data_to_transmit() override
                {
                    // Also check on connected state so that we don't try to send data
                    // when the socket just has been closed while in SocketDispatcher::tick()
                    return connected && tx_buffer.size() > 0;
                }

                bool create_socket();
                int socket_id = -1;
                std::shared_ptr<InetAddress> ip;
                bool started = false;
                bool connected = false;
                ITransferBuffer& tx_buffer;
                ITransferBuffer& rx_buffer;
        };
    }
}

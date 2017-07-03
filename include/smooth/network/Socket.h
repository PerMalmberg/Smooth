//
// Created by permal on 6/25/17.
//

#pragma once

#include <cstring>
#include <sys/socket.h>

#undef bind

#include "InetAddress.h"
#include "IDataAvailable.h"
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

                Socket(IDataAvailable& destination);

                virtual ~Socket()
                {
                }

                bool start(std::shared_ptr<InetAddress> ip);
                void stop() override;

                void transmit(std::shared_ptr<ITransferBuffer> buffer);
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
                    return send_buffer && send_buffer->size() > 0;
                }
                bool create_socket();
                int socket_id = -1;
                IDataAvailable& destination;
                std::shared_ptr<InetAddress> ip;
                bool started = false;
                bool connected = false;

                std::shared_ptr<ITransferBuffer> send_buffer = nullptr;
        };
    }
}

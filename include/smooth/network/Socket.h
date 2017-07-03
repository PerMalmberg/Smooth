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
#include "SendBuffer.h"

namespace smooth
{
    namespace network
    {
        class Socket
                : public ISocket
        {
            public:
                Socket(IDataAvailable& destination);

                virtual ~Socket()
                {
                }

                bool start(std::shared_ptr<InetAddress> ip);
                void stop() override;
                void transmit(std::shared_ptr<ISendBuffer> buffer);

                void readable() override;
                void writable() override;

                bool has_data_to_transmit() override
                {
                    return send_buffer && send_buffer->size() > 0;
                }

                void internal_start() override;
                bool is_started() override;
                int get_socket_id() override
                {
                    return socket_id;
                }
            private:
                bool set_non_blocking();
                bool create_socket();
                int socket_id = -1;
                IDataAvailable& destination;
                std::shared_ptr<InetAddress> ip;
                bool started = false;
                bool connected = false;
                std::shared_ptr<ISendBuffer> send_buffer = nullptr;
        };
    }
}

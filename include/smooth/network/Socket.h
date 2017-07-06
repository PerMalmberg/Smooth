//
// Created by permal on 6/25/17.
//

#pragma once

#include <cstring>
#include <sys/socket.h>

#undef bind

#include <array>
#include "InetAddress.h"
#include <memory>
#include "ISocket.h"
#include <smooth/util/CircularBuffer.h>
#include <smooth/ipc/TaskEventQueue.h>
#include <smooth/network/TransmitBufferEmpty.h>
#include <smooth/network/DataAvailable.h>

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

                Socket(util::ICircularBuffer<char>& tx_buffer, util::ICircularBuffer<char>& rx_buffer,
                       smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmpty>& tx_empty,
                       smooth::ipc::TaskEventQueue<smooth::network::DataAvailable>& data_available);

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

                bool is_connected() override
                {
                    return connected;
                }

                int get_socket_id() override
                {
                    return socket_id;
                }

                bool set_non_blocking();

                bool has_data_to_transmit() override
                {
                    // Also check on connected state so that we don't try to send data
                    // when the socket just has been closed while in SocketDispatcher::tick()
                    return connected && tx_buffer.available_items() > 0;
                }

                void check_if_connection_is_completed() override;

                bool create_socket();

                int socket_id = -1;
                std::shared_ptr<InetAddress> ip;
                bool started = false;
                bool connected = false;
                smooth::util::ICircularBuffer<char>& tx_buffer;
                smooth::util::ICircularBuffer<char>& rx_buffer;
                smooth::ipc::TaskEventQueue<smooth::network::TransmitBufferEmpty>& tx_empty;
                smooth::ipc::TaskEventQueue<smooth::network::DataAvailable>& data_available;
        };
    }
}

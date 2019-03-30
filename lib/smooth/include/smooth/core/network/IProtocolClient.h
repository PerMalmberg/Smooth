#pragma once

#include "IPacketSendBuffer.h"
#include "IPacketReceiveBuffer.h"
#include "event/TransmitBufferEmptyEvent.h"
#include "event/DataAvailableEvent.h"
#include "event/ConnectionStatusEvent.h"
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/network/ISocket.h>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            template<typename Protocol>
            class IProtocolClient
            {
                public:
                    virtual ~IProtocolClient() = default;

                    virtual IPacketSendBuffer<Protocol>& get_tx_buffer() = 0;

                    virtual IPacketReceiveBuffer<Protocol>& get_rx_buffer() = 0;

                    virtual smooth::core::ipc::TaskEventQueue<smooth::core::network::event::TransmitBufferEmptyEvent>&
                    get_tx_empty() = 0;

                    virtual smooth::core::ipc::TaskEventQueue<smooth::core::network::event::DataAvailableEvent<Protocol>>&
                    get_data_available() = 0;

                    virtual smooth::core::ipc::TaskEventQueue<smooth::core::network::event::ConnectionStatusEvent>&
                    get_connection_status() = 0;

                    virtual std::chrono::milliseconds get_send_timeout() = 0;

                    void set_socket(const std::shared_ptr<smooth::core::network::ISocket>& s)
                    {
                        socket = s;
                    }

                protected:
                    std::shared_ptr<smooth::core::network::ISocket> socket{};
            };
        }

    }
}
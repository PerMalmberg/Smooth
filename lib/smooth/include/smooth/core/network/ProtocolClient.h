#pragma once

#include "IPacketSendBuffer.h"
#include "IPacketReceiveBuffer.h"
#include "event/TransmitBufferEmptyEvent.h"
#include "event/DataAvailableEvent.h"
#include "event/ConnectionStatusEvent.h"
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/network/ISocket.h>
#include <smooth/core/network/BufferContainer.h>

namespace smooth
{
    namespace core
    {
        namespace network
        {
            template<typename Protocol>
            class ProtocolClient
                    : public smooth::core::ipc::IEventListener<smooth::core::network::event::DataAvailableEvent<Protocol>>,
                      public smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>,
                      public smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>
            {
                public:
                    ProtocolClient(smooth::core::Task& task,
                                   smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>& tx_empty,
                                   smooth::core::ipc::IEventListener<smooth::core::network::event::DataAvailableEvent<Protocol>>& data_available,
                                   smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>& connection_status);

                    ~ProtocolClient() override
                    {
                        container->clear();
                    };

                    void set_socket(const std::shared_ptr<smooth::core::network::ISocket>& s)
                    {
                        socket = s;
                    }

                    std::shared_ptr<BufferContainer<Protocol>> get_buffers()
                    {
                        return container;
                    }

                    virtual std::chrono::milliseconds get_send_timeout() = 0;

                    virtual void event(const smooth::core::network::event::DataAvailableEvent<Protocol>& event) = 0;

                    virtual void event(const smooth::core::network::event::TransmitBufferEmptyEvent& event) = 0;

                    virtual void event(const smooth::core::network::event::ConnectionStatusEvent& event) = 0;

                protected:
                    std::shared_ptr<smooth::core::network::ISocket> socket{};
                    std::shared_ptr<BufferContainer<Protocol>> container{};
            };

            template<typename Protocol>
            ProtocolClient<Protocol>::ProtocolClient(
                    smooth::core::Task& task,
                    smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>& tx_empty,
                    ipc::IEventListener<event::DataAvailableEvent<Protocol>>& data_available,
                    smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>& connection_status)
                    : container(std::make_shared<BufferContainer<Protocol>>(task,
                                                                            tx_empty,
                                                                            data_available,
                                                                            connection_status))
            {
            }
        }

    }
}
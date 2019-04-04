#pragma once

#include "IPacketSendBuffer.h"
#include "IPacketReceiveBuffer.h"
#include "event/TransmitBufferEmptyEvent.h"
#include "event/DataAvailableEvent.h"
#include "event/ConnectionStatusEvent.h"
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/network/ISocket.h>
#include <smooth/core/network/BufferContainer.h>
#include "ClientPool.h"

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
                    ProtocolClient(smooth::core::Task& task);

                    ~ProtocolClient() override = default;

                    void set_socket(const std::shared_ptr<smooth::core::network::ISocket>& s)
                    {
                        socket = s;
                    }

                    std::shared_ptr<BufferContainer<Protocol>> get_buffers()
                    {
                        return container;
                    }

                    void reset()
                    {
                        socket.reset();
                    }

                    virtual std::chrono::milliseconds get_send_timeout() = 0;

                    void event(const smooth::core::network::event::DataAvailableEvent<Protocol>& event) override = 0;

                    void event(const smooth::core::network::event::TransmitBufferEmptyEvent& event) override = 0;

                    void event(const smooth::core::network::event::ConnectionStatusEvent& event) override = 0;

                private:
                    std::shared_ptr<smooth::core::network::ISocket> socket{};
                    std::shared_ptr<BufferContainer<Protocol>> container;
            };

            template<typename Protocol>
            ProtocolClient<Protocol>::ProtocolClient(
                    smooth::core::Task& task)
                    : container(std::make_shared<BufferContainer<Protocol>>(task, *this, *this, *this))
            {
            }
        }

    }
}
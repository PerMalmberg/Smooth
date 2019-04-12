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
            template<typename Protocol, typename Client>
            class ProtocolClient
                    : public smooth::core::ipc::IEventListener<smooth::core::network::event::DataAvailableEvent<Protocol>>,
                      public smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>,
                      public smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>,
                      public std::enable_shared_from_this<Client>
            {
                public:
                    ProtocolClient(smooth::core::Task& task, smooth::core::network::ClientPool<Client>& pool);

                    ~ProtocolClient() override = default;
                    ProtocolClient(const ProtocolClient&) = delete;

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

                    virtual void connected() = 0;

                    virtual void disconnected() = 0;

                    void event(const smooth::core::network::event::ConnectionStatusEvent& event) final
                    {
                        if(event.is_connected())
                        {
                            connected();
                        }
                        else
                        {
                            this->disconnected();
                            pool.return_client(this->shared_from_this());
                        }
                    }

                private:
                    std::shared_ptr<smooth::core::network::ISocket> socket{};
                    smooth::core::network::ClientPool<Client>& pool;
                    std::shared_ptr<BufferContainer<Protocol>> container;
            };

            template<typename Protocol, typename Client>
            ProtocolClient<Protocol, Client>::ProtocolClient(
                    smooth::core::Task& task, smooth::core::network::ClientPool<Client>& pool)
                    : pool(pool),
                      container(std::make_shared<BufferContainer<Protocol>>(task, *this, *this, *this))
            {
            }
        }

    }
}
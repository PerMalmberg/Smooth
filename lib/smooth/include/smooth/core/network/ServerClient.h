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

namespace smooth::core::network
{
    template<typename Client, typename Protocol>
    class ServerSocket;

    template<typename Client, typename Protocol>
    class SecureServerSocket;

    /// ServerClient is the base class for all clients created by the ServerSocket. ServerClient provides
    /// the base functionality to implement a client capable of communicating over the associated socket.
    /// \tparam FinalClientTypeName The complete derived type of the client.
    /// \tparam Protocol The communication protocol
    template<typename FinalClientTypeName, typename Protocol>
    class ServerClient
            : public smooth::core::ipc::IEventListener<smooth::core::network::event::DataAvailableEvent<Protocol>>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>,
              public std::enable_shared_from_this<FinalClientTypeName>
    {
        public:
            ServerClient(smooth::core::Task& task, smooth::core::network::ClientPool<FinalClientTypeName>& pool);

            ~ServerClient() override = default;

            ServerClient(const ServerClient&) = delete;


            virtual std::chrono::milliseconds get_send_timeout() = 0;

            void event(const smooth::core::network::event::DataAvailableEvent<Protocol>& event) override = 0;

            void event(const smooth::core::network::event::TransmitBufferEmptyEvent& event) override = 0;

            virtual void connected() = 0;

            virtual void disconnected() = 0;

            virtual void reset_client() = 0;

            void event(const smooth::core::network::event::ConnectionStatusEvent& event) final
            {
                if (event.is_connected())
                {
                    connected();
                }
                else
                {
                    disconnected();
                    pool.return_client(this->shared_from_this());
                }
            }

            void set_client_context(void* ctx)
            {
                client_context = ctx;
            }

            void* get_client_context()
            {
                return client_context;
            }

            std::shared_ptr<BufferContainer<Protocol>> get_buffers()
            {
                return container;
            }

            void close()
            { socket->stop(); }

        private:
            friend ServerSocket<FinalClientTypeName, Protocol>;
            friend SecureServerSocket<FinalClientTypeName, Protocol>;
            friend ClientPool<FinalClientTypeName>;

            void set_socket(const std::shared_ptr<smooth::core::network::ISocket>& s)
            {
                socket = s;
            }

            void reset()
            {
                reset_client();
                socket.reset();
                get_buffers()->clear();
            }

            std::shared_ptr<smooth::core::network::ISocket> socket{};
            smooth::core::network::ClientPool<FinalClientTypeName>& pool;
            std::shared_ptr<BufferContainer<Protocol>> container;
            void* client_context{nullptr};
    };

    template<typename FinalClientTypeName, typename Protocol>
    ServerClient<FinalClientTypeName, Protocol>::ServerClient(
            smooth::core::Task& task, smooth::core::network::ClientPool<FinalClientTypeName>& pool)
            : pool(pool),
              container(std::make_shared<BufferContainer<Protocol>>(task, *this, *this, *this))
    {
    }
}
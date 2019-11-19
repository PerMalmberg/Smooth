/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <memory>
#include "IPacketSendBuffer.h"
#include "IPacketReceiveBuffer.h"
#include "event/TransmitBufferEmptyEvent.h"
#include "event/DataAvailableEvent.h"
#include "event/ConnectionStatusEvent.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/network/ISocket.h"
#include "smooth/core/network/BufferContainer.h"
#include "ClientPool.h"

namespace smooth::core::network
{
    template<typename Client, typename Protocol, typename ClientContext>
    class ServerSocket;

    template<typename Client, typename Protocol, typename ClientContext>
    class SecureServerSocket;

    /// ServerClient is the base class for all clients created by the ServerSocket. ServerClient provides
    /// the base functionality to implement a client capable of communicating over the associated socket.
    /// \tparam FinalClientTypeName The complete derived type of the client.
    /// \tparam Protocol The communication protocol
    template<typename FinalClientTypeName, typename Protocol, typename ClientContext>
    class ServerClient
        : public smooth::core::ipc::IEventListener<smooth::core::network::event::DataAvailableEvent<Protocol>>,
        public smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>,
        public smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>,
        public std::enable_shared_from_this<FinalClientTypeName>
    {
        public:
            ServerClient(smooth::core::Task& task,
                         smooth::core::network::ClientPool<FinalClientTypeName>& pool,
                         std::unique_ptr<Protocol> proto);

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

            void set_client_context(ClientContext* ctx)
            {
                client_context = ctx;
            }

            ClientContext* get_client_context()
            {
                return client_context;
            }

            std::weak_ptr<BufferContainer<Protocol>> get_buffers()
            {
                return container;
            }

            void close()
            {
                socket->stop("Server client closing");
            }

        protected:
            std::shared_ptr<smooth::core::network::ISocket> socket{};
            std::shared_ptr<BufferContainer<Protocol>> container;
        private:
            friend ServerSocket<FinalClientTypeName, Protocol, ClientContext>;
            friend SecureServerSocket<FinalClientTypeName, Protocol, ClientContext>;

            friend ClientPool<FinalClientTypeName>;

            void set_socket(const std::shared_ptr<smooth::core::network::ISocket>& s)
            {
                socket = s;
            }

            void reset()
            {
                reset_client();
                socket.reset();
                container->clear();
            }

            smooth::core::network::ClientPool<FinalClientTypeName>& pool;
            ClientContext* client_context{ nullptr };
    };

    template<typename FinalClientTypeName, typename Protocol, typename ClientContext>
    ServerClient<FinalClientTypeName, Protocol, ClientContext>::ServerClient(
        smooth::core::Task& task, smooth::core::network::ClientPool<FinalClientTypeName>& pool,
        std::unique_ptr<Protocol> proto)
            : container(std::make_shared<BufferContainer<Protocol>>(task, *this, *this, *this, std::move(proto))),
              pool(pool)
    {
    }
}

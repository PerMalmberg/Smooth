#pragma once

#include <functional>
#include <smooth/core/Application.h>
#include <smooth/core/network/SecureSocket.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/network/event/ClientConnectedEvent.h>
#include <smooth/core/network/Socket.h>
#include <smooth/core/network/event/ClientConnectedEvent.h>
#include <smooth/core/network/ServerSocket.h>
#include "StreamingProtocol.h"
#include "StreamingClient.h"

namespace server_socket_test
{
    class App
            : public smooth::core::Application,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::ClientConnectedEvent<StreamingClient>>
    {
        public:
            App();

            void init() override;
            void tick() override;

            void event(const smooth::core::network::event::ClientConnectedEvent<StreamingClient>& ev) override;

        private:
            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::ClientConnectedEvent<StreamingClient>> client_connected;
            std::shared_ptr<smooth::core::network::ServerSocket<StreamingProtocol, StreamingClient>> server{};
    };
}
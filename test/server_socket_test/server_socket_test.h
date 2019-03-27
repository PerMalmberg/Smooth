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

namespace server_socket_test
{
    class App
            : public smooth::core::Application,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::ClientConnectedEvent<StreamingProtocol>>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::DataAvailableEvent<StreamingProtocol>>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>
    {
        public:
            App();

            void init() override;
            void tick() override;

            void event(const smooth::core::network::event::ClientConnectedEvent<StreamingProtocol>& ev) override;
            void event(const smooth::core::network::event::TransmitBufferEmptyEvent&) override;
            void event(const smooth::core::network::event::DataAvailableEvent<StreamingProtocol>&) override;
            void event(const smooth::core::network::event::ConnectionStatusEvent&) override;

        private:
            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::ClientConnectedEvent<StreamingProtocol>> client_connected;
            std::shared_ptr<smooth::core::network::ISocket> server{};

            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::TransmitBufferEmptyEvent> tx_empty;
            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::DataAvailableEvent<StreamingProtocol>> data_available;
            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::ConnectionStatusEvent> connection_status;
            smooth::core::network::PacketSendBuffer<StreamingProtocol, 5> tx_buffer{};
            smooth::core::network::PacketReceiveBuffer<StreamingProtocol, 5> rx_buffer{};
    };
}
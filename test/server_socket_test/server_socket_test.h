#pragma once

#include <functional>
#include <smooth/core/Application.h>
#include <smooth/core/network/SecureSocket.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/network/ClientConnectedEvent.h>
#include <smooth/core/network/Socket.h>
#include <smooth/core/network/ClientConnectedEvent.h>
#include <smooth/core/network/ServerSocket.h>
#include "StreamingProtocol.h"

namespace server_socket_test
{
    class App
            : public smooth::core::Application,
              public smooth::core::ipc::IEventListener<smooth::core::network::ClientConnectedEvent<StreamingProtocol>>,
              public smooth::core::ipc::IEventListener<smooth::core::network::TransmitBufferEmptyEvent>,
              public smooth::core::ipc::IEventListener<smooth::core::network::DataAvailableEvent<StreamingProtocol>>,
              public smooth::core::ipc::IEventListener<smooth::core::network::ConnectionStatusEvent>
    {
        public:
            App();

            void init() override;
            void tick() override;

            void event(const smooth::core::network::ClientConnectedEvent<StreamingProtocol>& ev) override;
            void event(const smooth::core::network::TransmitBufferEmptyEvent&) override;
            void event(const smooth::core::network::DataAvailableEvent<StreamingProtocol>&) override;
            void event(const smooth::core::network::ConnectionStatusEvent&) override;

        private:
            smooth::core::ipc::TaskEventQueue<smooth::core::network::ClientConnectedEvent<StreamingProtocol>> client_connected;
            std::shared_ptr<smooth::core::network::ISocket> server{};

            smooth::core::ipc::TaskEventQueue<smooth::core::network::TransmitBufferEmptyEvent> tx_empty;
            smooth::core::ipc::TaskEventQueue<smooth::core::network::DataAvailableEvent<StreamingProtocol>> data_available;
            smooth::core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent> connection_status;
            smooth::core::network::PacketSendBuffer<StreamingProtocol, 5> tx_buffer{};
            smooth::core::network::PacketReceiveBuffer<StreamingProtocol, 5> rx_buffer{};
    };
}
#pragma once

#include <functional>
#include <smooth/core/Application.h>
#include <smooth/core/network/SecureSocket.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/network/event/ConnectionStatusEvent.h>
#include <smooth/core/network/Socket.h>
#include <smooth/core/network/IPv4.h>
#include "HTTPProtocol.h"

namespace secure_socket_test
{
    class App
            : public smooth::core::Application,
    public smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::DataAvailableEvent<HTTPProtocol<>>>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>


    {
        public:
            App();

            void init() override;
            void tick() override;

            void event(const smooth::core::network::event::TransmitBufferEmptyEvent&) override;
            void event(const smooth::core::network::event::DataAvailableEvent<HTTPProtocol<>>&) override;
            void event(const smooth::core::network::event::ConnectionStatusEvent&) override;

        private:
            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::TransmitBufferEmptyEvent> tx_empty;
            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::DataAvailableEvent<HTTPProtocol<>>> data_available;
            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::ConnectionStatusEvent> connection_status;
            smooth::core::network::PacketSendBuffer<HTTPProtocol<>, 5> tx_buffer{};
            smooth::core::network::PacketReceiveBuffer<HTTPProtocol<>, 5> rx_buffer{};
            std::shared_ptr<smooth::core::network::ISocket> sock{};
            std::vector<uint8_t> received_content{};
    };
}
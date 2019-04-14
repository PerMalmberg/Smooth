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
            std::shared_ptr<smooth::core::network::BufferContainer<HTTPProtocol<>>> buff;
            std::shared_ptr<smooth::core::network::SecureSocket<HTTPProtocol<>>> sock{};
            std::vector<uint8_t> received_content{};
    };
}
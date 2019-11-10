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

#include <functional>
#include "smooth/core/Application.h"
#include "smooth/core/network/SecureSocket.h"
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/network/event/ConnectionStatusEvent.h"
#include "smooth/core/network/Socket.h"
#include "smooth/core/network/IPv4.h"
#include "smooth/application/network/http/HTTPProtocol.h"
#include "smooth/application/network/http/IResponseOperation.h"

namespace secure_socket_test
{
    using Proto = smooth::application::network::http::HTTPProtocol;

    class App
        : public smooth::core::Application,
        public smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>,
        public smooth::core::ipc::IEventListener<smooth::core::network::event::DataAvailableEvent<Proto>>,
        public smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>,
        public smooth::core::ipc::IEventListener<smooth::core::network::NetworkStatus>,
        public smooth::application::network::http::IServerResponse
    {
        public:
            App();

            void init() override;

            void tick() override;

            void event(const smooth::core::network::event::TransmitBufferEmptyEvent&) override;

            void event(const smooth::core::network::event::DataAvailableEvent<Proto>&) override;

            void event(const smooth::core::network::event::ConnectionStatusEvent&) override;

            void event(const smooth::core::network::NetworkStatus& event) override;

            void reply(std::unique_ptr<smooth::application::network::http::IResponseOperation>, bool) override
            {}

            void reply_error(std::unique_ptr<smooth::application::network::http::IResponseOperation> ) override
            {}

            smooth::core::Task& get_task() override
            {
                return *this;
            }

            void upgrade_to_websocket_internal() override
            {}
        private:
            std::shared_ptr<smooth::core::network::BufferContainer<Proto>> buff;
            std::shared_ptr<smooth::core::network::SecureSocket<Proto>> sock{};
            std::vector<uint8_t> received_content{};
            std::unique_ptr<smooth::core::network::MBedTLSContext> tls_context{};
            using NetworkStatusQueue =
                smooth::core::ipc::SubscribingTaskEventQueue<smooth::core::network::NetworkStatus>;
            std::shared_ptr<NetworkStatusQueue> net_status;

            std::unique_ptr<std::vector<unsigned char>> get_certs() const;
    };
}

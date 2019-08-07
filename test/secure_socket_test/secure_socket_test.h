// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <functional>
#include <smooth/core/Application.h>
#include <smooth/core/network/SecureSocket.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/network/event/ConnectionStatusEvent.h>
#include <smooth/core/network/Socket.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/application/network/http/HTTPProtocol.h>
#include <smooth/application/network/http/IResponseOperation.h>

namespace secure_socket_test
{
    using Proto = smooth::application::network::http::HTTPProtocol;

    class App
            : public smooth::core::Application,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::DataAvailableEvent<Proto>>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>,
              public smooth::application::network::http::IServerResponse
    {
        public:
            App();

            void init() override;

            void tick() override;

            void event(const smooth::core::network::event::TransmitBufferEmptyEvent&) override;

            void event(const smooth::core::network::event::DataAvailableEvent<Proto>&) override;

            void event(const smooth::core::network::event::ConnectionStatusEvent&) override;

            void reply(std::unique_ptr<smooth::application::network::http::IResponseOperation>, bool) override
            {};

            void reply_error(std::unique_ptr<smooth::application::network::http::IResponseOperation>) override
            {};

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

            std::unique_ptr<std::vector<unsigned char>> get_certs() const;
    };
}
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

#include <smooth/core/network/ServerClient.h>
#include <chrono>
#include "StreamingProtocol.h"
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/network/BufferContainer.h>
#include <smooth/core/network/event/DataAvailableEvent.h>
#include <smooth/core/logging/log.h>

namespace server_socket_test
{
    class StreamingClient
        : public smooth::core::network::ServerClient<StreamingClient, StreamingProtocol, void>
    {
        public:
            explicit StreamingClient(smooth::core::Task& task, smooth::core::network::ClientPool<StreamingClient>& pool)
                    : ServerClient<StreamingClient, StreamingProtocol, void>(task, pool,
                                                                             std::make_unique<StreamingProtocol>())
            {
            }

            ~StreamingClient() override = default;

            void event(const smooth::core::network::event::DataAvailableEvent<StreamingProtocol>& event) override
            {
                // Print data as it is received.
                StreamingProtocol::packet_type packet;

                if (event.get(packet))
                {
                    std::string s{ static_cast<char>(packet.data()[0]) };
                    smooth::core::logging::Log::debug("-->", s);
                }
            }

            void event(const smooth::core::network::event::TransmitBufferEmptyEvent& /*event*/) override
            {
            }

            void disconnected() override
            {
            }

            void connected() override
            {
            }

            void reset_client() override
            {
            }

            std::chrono::milliseconds get_send_timeout() override
            {
                return std::chrono::seconds{ 1 };
            }
    };
}

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

#include "smooth/core/network/ServerClient.h"
#include <chrono>
#include "StreamingProtocol.h"
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/network/BufferContainer.h"
#include "smooth/core/network/event/DataAvailableEvent.h"
#include "smooth/core/logging/log.h"

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

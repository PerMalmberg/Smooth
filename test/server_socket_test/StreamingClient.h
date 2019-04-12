#pragma once

#include <smooth/core/network/ProtocolClient.h>
#include <chrono>
#include "StreamingProtocol.h"
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/network/BufferContainer.h>
#include <smooth/core/network/event/DataAvailableEvent.h>
#include <smooth/core/logging/log.h>

namespace server_socket_test
{

    class StreamingClient
            : public smooth::core::network::ProtocolClient<StreamingProtocol, StreamingClient>
    {
        public:
            explicit StreamingClient(smooth::core::Task& task, smooth::core::network::ClientPool<StreamingClient>& pool)
                    : ProtocolClient<StreamingProtocol, StreamingClient>(task, pool)

            {
            }

            ~StreamingClient() override = default;

            void event(const smooth::core::network::event::DataAvailableEvent<StreamingProtocol>& event) override
            {
                StreamingProtocol::packet_type packet;
                event.get(packet);
                std::string s{static_cast<char>(packet.data()[0])};
                smooth::core::logging::Log::debug("-->", s);
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


            std::chrono::milliseconds get_send_timeout() override
            {
                return std::chrono::seconds{1};
            };
    };


}
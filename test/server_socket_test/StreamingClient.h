#pragma once

#include <smooth/core/network/IProtocolClient.h>
#include <chrono>
#include "StreamingProtocol.h"
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/network/event/DataAvailableEvent.h>

namespace server_socket_test
{

    class StreamingClient
            : public smooth::core::network::IProtocolClient<StreamingProtocol>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::DataAvailableEvent<StreamingProtocol>>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::TransmitBufferEmptyEvent>,
              public smooth::core::ipc::IEventListener<smooth::core::network::event::ConnectionStatusEvent>
    {
        public:
            explicit StreamingClient(smooth::core::Task& task)
                    :
                    tx_empty("tx_empty", 3, task, *this),
                    data_available("data_available", 3, task, *this),
                    connection_status("connection_status", 3, task, *this)
            {
            }

            ~StreamingClient() override
            {

            }

            void event(const smooth::core::network::event::DataAvailableEvent<StreamingProtocol>& event) override
            {

            }

            void event(const smooth::core::network::event::TransmitBufferEmptyEvent& event) override
            {

            }

            void event(const smooth::core::network::event::ConnectionStatusEvent& event) override
            {

            }


            smooth::core::network::IPacketSendBuffer<StreamingProtocol>& get_tx_buffer() override
            {
                return tx_buffer;
            }

            smooth::core::network::IPacketReceiveBuffer<StreamingProtocol>& get_rx_buffer() override
            {
                return rx_buffer;
            }

            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::TransmitBufferEmptyEvent>&
            get_tx_empty() override
            {
                return tx_empty;
            }

            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::DataAvailableEvent<StreamingProtocol>>&
            get_data_available() override
            {
                return data_available;
            }

            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::ConnectionStatusEvent>&
            get_connection_status() override
            {
                return connection_status;
            }

            std::chrono::milliseconds get_send_timeout() override
            {
                return std::chrono::seconds{1};
            };

        private:
            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::TransmitBufferEmptyEvent> tx_empty;
            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::DataAvailableEvent<StreamingProtocol>> data_available;
            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::ConnectionStatusEvent> connection_status;
            smooth::core::network::PacketSendBuffer<StreamingProtocol, 5> tx_buffer{};
            smooth::core::network::PacketReceiveBuffer<StreamingProtocol, 5> rx_buffer{};
    };


}
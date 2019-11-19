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

#include <memory>
#include "smooth/core/Task.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/network/PacketSendBuffer.h"
#include "smooth/core/network/PacketReceiveBuffer.h"
#include "smooth/core/network/event/TransmitBufferEmptyEvent.h"
#include "smooth/core/network/event/ConnectionStatusEvent.h"
#include "smooth/core/network/event/DataAvailableEvent.h"
#include "smooth/core/logging/log.h"

namespace smooth::core::network
{
    template<typename Protocol, int BufferSize = 5>
    class BufferContainer
    {
        public:
            BufferContainer(smooth::core::Task& task,
                            smooth::core::ipc::IEventListener<event::TransmitBufferEmptyEvent>& transmit_buffer_empty,
                            smooth::core::ipc::IEventListener<event::DataAvailableEvent<Protocol>>& data_receiver,
                            smooth::core::ipc::IEventListener<event::ConnectionStatusEvent>& connection_status_receiver,
                            std::unique_ptr<Protocol> proto);

            const auto& get_tx_empty()
            {
                return tx_empty;
            }

            const auto& get_data_available()
            {
                return data_available;
            }

            const auto& get_connection_status()
            {
                return connection_status;
            }

            smooth::core::network::PacketSendBuffer<Protocol, BufferSize>& get_tx_buffer()
            {
                return tx_buffer;
            }

            smooth::core::network::PacketReceiveBuffer<Protocol, BufferSize>& get_rx_buffer()
            {
                return rx_buffer;
            }

            void clear()
            {
                rx_buffer.clear();
                tx_buffer.clear();
                data_available->clear();
                tx_empty->clear();
                connection_status->clear();
            }

            Protocol& get_protocol() const
            {
                return rx_buffer.get_proto();
            }

        private:
            using TxEmptyQueue = smooth::core::ipc::TaskEventQueue<event::TransmitBufferEmptyEvent>;
            std::shared_ptr<TxEmptyQueue> tx_empty;
            using DataAvailableQueue = smooth::core::ipc::TaskEventQueue<event::DataAvailableEvent<Protocol>>;
            std::shared_ptr<DataAvailableQueue> data_available;
            using ConnectionStatusQueue = smooth::core::ipc::TaskEventQueue<network::event::ConnectionStatusEvent>;
            std::shared_ptr<ConnectionStatusQueue> connection_status;
            PacketSendBuffer<Protocol, BufferSize> tx_buffer{};
            PacketReceiveBuffer<Protocol, BufferSize> rx_buffer{};
    };

    template<typename Protocol, int BufferSize>
    BufferContainer<Protocol, BufferSize>::BufferContainer(smooth::core::Task& task,
                                                           smooth::core::ipc::IEventListener<event::TransmitBufferEmptyEvent>& transmit_buffer_empty,
                                                           smooth::core::ipc::IEventListener<event::DataAvailableEvent<Protocol>>& data_receiver,
                                                           smooth::core::ipc::IEventListener<event::ConnectionStatusEvent>& connection_status_receiver,
                                                           std::unique_ptr<Protocol> proto)
            : tx_empty(TxEmptyQueue::create(BufferSize, task, transmit_buffer_empty)),
              data_available(DataAvailableQueue::create(BufferSize, task, data_receiver)),
              connection_status(ConnectionStatusQueue::create(BufferSize, task, connection_status_receiver)),
              rx_buffer(std::move(proto))
    {
    }
}

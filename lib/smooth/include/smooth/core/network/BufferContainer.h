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

#include <memory>
#include <smooth/core/Task.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/network/PacketSendBuffer.h>
#include <smooth/core/network/PacketReceiveBuffer.h>
#include <smooth/core/network/event/TransmitBufferEmptyEvent.h>
#include <smooth/core/network/event/ConnectionStatusEvent.h>
#include <smooth/core/network/event/DataAvailableEvent.h>
#include <smooth/core/logging/log.h>

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


            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::TransmitBufferEmptyEvent>&
            get_tx_empty()
            {
                return tx_empty;
            }

            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::DataAvailableEvent<Protocol>>&
            get_data_available()
            {
                return data_available;
            }

            smooth::core::ipc::TaskEventQueue<smooth::core::network::event::ConnectionStatusEvent>&
            get_connection_status()
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
                data_available.clear();
                tx_empty.clear();
                connection_status.clear();
            }


        private:
            smooth::core::ipc::TaskEventQueue<event::TransmitBufferEmptyEvent> tx_empty;
            smooth::core::ipc::TaskEventQueue<event::DataAvailableEvent<Protocol>> data_available;
            smooth::core::ipc::TaskEventQueue<network::event::ConnectionStatusEvent> connection_status;
            PacketSendBuffer<Protocol, BufferSize> tx_buffer{};
            PacketReceiveBuffer<Protocol, BufferSize> rx_buffer{};
    };

    template<typename Protocol, int BufferSize>
    BufferContainer<Protocol, BufferSize>::BufferContainer(smooth::core::Task& task,
                                                           smooth::core::ipc::IEventListener<event::TransmitBufferEmptyEvent>& transmit_buffer_empty,
                                                           smooth::core::ipc::IEventListener<event::DataAvailableEvent<Protocol>>& data_receiver,
                                                           smooth::core::ipc::IEventListener<event::ConnectionStatusEvent>& connection_status_receiver,
                                                           std::unique_ptr<Protocol> proto)
            : tx_empty("", BufferSize, task, transmit_buffer_empty),
              data_available("", BufferSize, task, data_receiver),
              connection_status("", BufferSize, task, connection_status_receiver),
              rx_buffer(std::move(proto))
    {
    }
}
#include <utility>

#include <utility>

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

namespace smooth
{
    namespace core
    {
        namespace network
        {
            template<typename Protocol, int BufferSize = 5>
            class BufferContainer
            {
                public:
                    BufferContainer(smooth::core::Task& task,
                                    smooth::core::ipc::IEventListener<event::TransmitBufferEmptyEvent>& transmit_buffer_empty,
                                    smooth::core::ipc::IEventListener<event::DataAvailableEvent<Protocol>>& data_receiver,
                                    smooth::core::ipc::IEventListener<event::ConnectionStatusEvent>& connection_status_receiver);


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
                                                                   smooth::core::ipc::IEventListener<event::ConnectionStatusEvent>& connection_status_receiver)
                    : tx_empty("", BufferSize, task, transmit_buffer_empty),
                      data_available("", BufferSize, task, data_receiver),
                      connection_status("", BufferSize, task, connection_status_receiver)
            {
            }
        }
    }
}
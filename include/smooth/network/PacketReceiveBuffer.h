//
// Created by permal on 7/9/17.
//

#pragma once

#include <smooth/util/CircularBuffer.h>
#include <smooth/ipc/Mutex.h>
#include "IPacketReceiveBuffer.h"

namespace smooth
{
    namespace network
    {
        // PacketReceiveBuffer is a buffer that can hold Size items of type T
        // and helps with assembling of data packets.

        // T must provide the IReceivablePacket interface (either directly or via inheritance)
        // and fulfill the following contract:
        // * Default constructable
        // * Must be copyable

        template<typename T, int Size>
        class PacketReceiveBuffer
                : public IPacketReceiveBuffer<T>
        {
            public:
                PacketReceiveBuffer()
                        : guard(), current_item(), buffer()
                {
                }

                bool is_full() override
                {
                    smooth::ipc::Mutex::Lock lock(guard);
                    return buffer.is_full();
                }

                int amount_wanted() override
                {
                    return current_item.get_wanted_amount();
                }

                char* get_write_pos() override
                {
                    return current_item.get_write_pos();
                }

                void data_received(int length) override
                {
                    current_item.data_received(length);
                    if( current_item.is_complete())
                    {
                        buffer.put(current_item);
                        in_progress = false;
                    }
                }

                bool is_packet_complete() override
                {
                    return current_item.is_complete();
                }

                bool get(T& target) override
                {
                    smooth::ipc::Mutex::Lock lock(guard);
                    return buffer.get(target);
                }

                void clear() override
                {
                    smooth::ipc::Mutex::Lock lock(guard);
                    buffer.clear();
                    in_progress = false;
                }

                bool is_in_progress() override
                {
                    return in_progress;
                }

                void prepare_new_packet() override
                {
                    // Use placement new to prepare a new instance, but first destroy the current one.
                    current_item.~T();
                    new( &current_item ) T();
                    in_progress = true;
                }

                bool is_error() override
                {
                    return current_item.is_error();
                }

            private:
                smooth::ipc::Mutex guard;
                bool in_progress = false;
                T current_item;
                smooth::util::CircularBuffer<T, Size> buffer;
        };
    }
}

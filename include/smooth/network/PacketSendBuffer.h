//
// Created by permal on 7/8/17.
//

#pragma once

#include <smooth/util/CircularBuffer.h>
#include <smooth/ipc/Mutex.h>
#include "IPacketSendBuffer.h"

namespace smooth
{
    namespace network
    {
        // PacketSendBuffer is a buffer that can hold Size packets of type T, with
        // byte access to each individual element which makes it easy to perform
        // send() operations directly on for each packet.

        // T must provide the ISendablePacket interface (either directly or via inheritance) and fulfill the following contract:
        // * Default constructable
        // * Must be copyable

        template<typename T, int Size>
        class PacketSendBuffer
                : public IPacketSendBuffer<T>
        {
            public:
                PacketSendBuffer()
                        : buffer(), current_item(), guard()
                {
                }

                void put(const T& item)
                {
                    smooth::ipc::Mutex::Lock lock(guard);
                    buffer.put(item);
                }

                bool is_in_progress() override
                {
                    return in_progress;
                }

                const uint8_t* get_data_to_send() override
                {
                    return current_item.get_data() + current_item.get_send_length() - current_length;
                }

                int get_remaining_data_length() override
                {
                    return current_length;
                }

                void data_has_been_sent(int length) override
                {
                    current_length -= length;

                    // Just to be safe, we check for <= 0
                    if (current_length <= 0)
                    {
                        in_progress = false;
                        current_length = 0;
                    }
                }

                void prepare_next_packet() override
                {
                    smooth::ipc::Mutex::Lock lock(guard);
                    if (buffer.get(current_item))
                    {
                        in_progress = true;
                        current_length = current_item.get_send_length();
                    }
                }

                void clear() override
                {
                    smooth::ipc::Mutex::Lock lock(guard);
                    buffer.clear();
                    in_progress = false;
                    current_length = 0;
                }

                bool is_empty() override
                {
                    smooth::ipc::Mutex::Lock lock(guard);
                    return !in_progress && buffer.is_empty();
                }


            private:
                smooth::util::CircularBuffer<T, Size> buffer;
                T current_item;
                smooth::ipc::Mutex guard;
                int current_length = 0;
                bool in_progress = false;


        };


    }
}

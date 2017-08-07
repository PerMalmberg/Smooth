//
// Created by permal on 7/8/17.
//

#pragma once

#include <smooth/core/util/CircularBuffer.h>
#include <smooth/core/ipc/Mutex.h>
#include "IPacketSendBuffer.h"

namespace smooth
{
    namespace core
    {
        namespace network
        {
            /// PacketSendBuffer is a buffer that can hold Size packets of type T, with
            /// byte access to each individual element which makes it easy to perform
            /// send() operations directly on each packet.
            /// T must provide the IPacketDisassembly interface (either directly or via inheritance) and fulfill the following contract:
            /// * Default constructable
            /// * Must be copyable
            /// \tparam Packet The packet type
            /// \tparam Size Number of items to hold in the buffer
            template<typename Packet, int Size>
            class PacketSendBuffer
                    : public IPacketSendBuffer<Packet>
            {
                public:
                    PacketSendBuffer()
                            : buffer(), current_item(), guard()
                    {
                    }

                    void put(const Packet& item)
                    {
                        smooth::core::ipc::Mutex::Lock lock(guard);
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
                        smooth::core::ipc::Mutex::Lock lock(guard);
                        if (buffer.get(current_item))
                        {
                            in_progress = true;
                            current_length = current_item.get_send_length();
                        }
                    }

                    void clear() override
                    {
                        smooth::core::ipc::Mutex::Lock lock(guard);
                        buffer.clear();
                        in_progress = false;
                        current_length = 0;
                    }

                    bool is_empty() override
                    {
                        smooth::core::ipc::Mutex::Lock lock(guard);
                        return !in_progress && buffer.is_empty();
                    }


                private:
                    smooth::core::util::CircularBuffer<Packet, Size> buffer;
                    Packet current_item;
                    smooth::core::ipc::Mutex guard;
                    int current_length = 0;
                    bool in_progress = false;


            };


        }
    }
}
//
// Created by permal on 7/8/17.
//

#pragma once

#include <smooth/core/util/CircularBuffer.h>
#include "IPacketSendBuffer.h"
#include <mutex>

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

                    bool put(const Packet& item)
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        bool res = !buffer.is_full();
                        if (res)
                        {
                            buffer.put(item);
                        }
                        return res;
                    }

                    bool is_in_progress() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        return in_progress;
                    }

                    const uint8_t* get_data_to_send() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        return current_item.get_data() + bytes_sent;
                    }

                    int get_remaining_data_length() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        return current_item.get_send_length() - bytes_sent;
                    }

                    void data_has_been_sent(int length) override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        bytes_sent += length;
                        if(current_item.get_send_length() >= bytes_sent)
                        {
                            in_progress = false;
                        }
                    }

                    void prepare_next_packet() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        in_progress = buffer.get(current_item);
                        bytes_sent = 0;
                    }

                    void clear() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        buffer.clear();
                        in_progress = false;
                        bytes_sent = 0;
                    }

                    bool is_empty() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        return !in_progress && buffer.is_empty();
                    }


                private:
                    smooth::core::util::CircularBuffer<Packet, Size> buffer;
                    Packet current_item;
                    std::mutex guard;
                    int bytes_sent = 0;
                    bool in_progress = false;


            };


        }
    }
}
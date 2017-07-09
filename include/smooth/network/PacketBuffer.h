//
// Created by permal on 7/8/17.
//

#pragma once

#include <smooth/util/CircularBuffer.h>
#include <algorithm>
#include <smooth/ipc/Mutex.h>

namespace smooth
{
    namespace network
    {
        template<typename T>
        class IPacketBuffer
        {
            public:
                virtual ~IPacketBuffer()
                {
                }

                virtual bool is_in_progress() = 0;
                virtual const char* get_data_to_send() = 0;
                virtual int get_remaining_data_length(int max) = 0;
                virtual void data_has_been_sent(int length) = 0;
                virtual void prepare_next_packet() = 0;
                virtual void put(const T& item) = 0;
                virtual void clear() = 0;
                virtual bool is_empty() = 0;
        };

        // PacketBuffer is a buffer that can hold Size packets of type T, with
        // byte access to each individual element which makes it easy to perform
        // send() operations directly on for each packet.
        template<typename T, int Size>
        class PacketBuffer
                : public IPacketBuffer<T>
        {
            public:
                PacketBuffer()
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

                const char* get_data_to_send() override
                {
                    return current_item.get_data() + current_item.get_length() - current_length;
                }

                int get_remaining_data_length(int max) override
                {
                    return std::min(max, current_length);
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
                        current_length = current_item.get_length();
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

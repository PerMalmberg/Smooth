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

#include "smooth/core/util/CircularBuffer.h"
#include "IPacketSendBuffer.h"
#include <mutex>

namespace smooth::core::network
{
    /// PacketSendBuffer is a buffer that can hold Size packets of type T, with
    /// byte access to each individual element which makes it easy to perform
    /// send() operations directly on each packet.
    /// T must provide the IPacketDisassembly interface (either directly or via inheritance) and fulfill the following
    // contract:
    /// * Default constructable
    /// * Must be copyable
    /// \tparam Packet The packet type
    /// \tparam Size Number of items to hold in the buffer
    template<typename Protocol, int Size, typename Packet = typename Protocol::packet_type>
    class PacketSendBuffer
        : public IPacketSendBuffer<Protocol>
    {
        public:
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

                if (bytes_sent >= current_item.get_send_length())
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
            Packet current_item{};
            std::mutex guard{};
            int bytes_sent = 0;
            bool in_progress = false;
            smooth::core::util::CircularBuffer<Packet, Size> buffer{};
    };
}

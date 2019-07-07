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

#include <smooth/core/util/CircularBuffer.h>
#include "IPacketSendBuffer.h"
#include <mutex>

namespace smooth::core::network
{
    /// PacketSendBuffer is a buffer that can hold Size packets of type T, with
    /// byte access to each individual element which makes it easy to perform
    /// send() operations directly on each packet.
    /// T must provide the IPacketDisassembly interface (either directly or via inheritance) and fulfill the following contract:
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
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

#include <mutex>
#include <memory>
#include <smooth/core/util/CircularBuffer.h>
#include "IPacketReceiveBuffer.h"

namespace smooth::core::network
{
    /// PacketReceiveBuffer is a buffer that can hold Size items of type Packet
    /// and helps with assembling of data packets.
    /// Packet must provide the IPacketAssembly interface (either directly or via inheritance)
    /// and fulfill the following contract:
    /// * Default constructable
    /// * Must be copyable
    /// \tparam Packet The type of packet to assemble
    /// \tparam Size  The Number of items to hold in the buffer.
    template<typename Protocol, int Size, typename Packet = typename Protocol::packet_type>
    class PacketReceiveBuffer
            : public IPacketReceiveBuffer<Protocol>
    {
        public:
            explicit PacketReceiveBuffer(std::unique_ptr<Protocol> proto)
                    : proto(std::move(proto))
            {
            }

            bool is_full() override
            {
                std::unique_lock<std::mutex> lock(guard);
                return buffer.is_full();
            }

            int amount_wanted() override
            {
                std::unique_lock<std::mutex> lock(guard);
                return proto->get_wanted_amount(current_item);
            }

            LockedWritePos get_write_pos() override
            {
                LockedWritePos lock(guard);
                lock.set_pos(proto->get_write_pos(current_item));
                return lock;
            }

            void data_received(int length) override
            {
                std::unique_lock<std::mutex> lock(guard);
                proto->data_received(current_item, length);
                if (proto->is_complete(current_item))
                {
                    buffer.put(current_item);
                    in_progress = false;
                }
            }

            bool is_packet_complete() override
            {
                std::unique_lock<std::mutex> lock(guard);
                return proto->is_complete(current_item);
            }

            bool get(Packet& target) override
            {
                std::unique_lock<std::mutex> lock(guard);
                return buffer.get(target);
            }

            void clear() override
            {
                std::unique_lock<std::mutex> lock(guard);
                buffer.clear();
                // Clear out any packets in progress too.
                in_progress = false;
                ReplacePacketWithDefault();
                // Reset protocol so that it isn't left in a state
                // where it thinks it is in the middle of a receive.
                proto->reset();
            }

            bool is_in_progress() override
            {
                std::unique_lock<std::mutex> lock(guard);
                return in_progress;
            }

            void prepare_new_packet() override
            {
                std::unique_lock<std::mutex> lock(guard);
                ReplacePacketWithDefault();
                in_progress = true;
                proto->packet_consumed();
            }

            bool is_error() override
            {
                std::unique_lock<std::mutex> lock(guard);
                return proto->is_error();
            }

            Protocol& get_proto() const
            {
                return *proto;
            }

        private:
            void ReplacePacketWithDefault()
            {
                current_item.~Packet();
                new(&current_item) Packet();
            }

            std::mutex guard{};
            bool in_progress = false;
            Packet current_item{};
            std::unique_ptr<Protocol> proto;
            smooth::core::util::CircularBuffer<Packet, Size> buffer{};
    };
}
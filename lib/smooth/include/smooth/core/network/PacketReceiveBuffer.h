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

#include <mutex>
#include <memory>
#include "smooth/core/util/CircularBuffer.h"
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

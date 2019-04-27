//
// Created by permal on 7/9/17.
//

#pragma once

#include <mutex>
#include <smooth/core/util/CircularBuffer.h>
#include "IPacketReceiveBuffer.h"

namespace smooth
{
    namespace core
    {
        namespace network
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
                    PacketReceiveBuffer()
                            : proto()
                    {
                    }

                    bool is_full() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        return buffer.is_full();
                    }

                    int amount_wanted() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        return proto.get_wanted_amount(current_item);
                    }

                    uint8_t* get_write_pos() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        return proto.get_write_pos(current_item);
                    }

                    void data_received(int length) override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        proto.data_received(current_item, length);
                        if (proto.is_complete())
                        {
                            buffer.put(current_item);
                            in_progress = false;
                        }
                    }

                    bool is_packet_complete() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        return proto.is_complete();
                    }

                    bool get(Packet& target) override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        return buffer.get(target);
                    }

                    void clear() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        buffer.clear();
                        // Clear out any packets in progress too.
                        in_progress = false;
                        ReplacePacketWithDefault();
                    }

                    bool is_in_progress() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        return in_progress;
                    }

                    void prepare_new_packet() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        ReplacePacketWithDefault();
                        in_progress = true;
                        proto.packet_consumed();
                    }

                    void ReplacePacketWithDefault()
                    {
                        current_item.~Packet();
                        new(&current_item) Packet();
                    }

                    bool is_error() override
                    {
                        std::lock_guard<std::mutex> lock(guard);
                        return proto.is_error();
                    }

                private:
                    std::mutex guard{};
                    bool in_progress = false;
                    Packet current_item{};
                    Protocol proto;
                    smooth::core::util::CircularBuffer<Packet, Size> buffer{};
            };
        }
    }
}
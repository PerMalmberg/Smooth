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

#include <cstdint>

namespace smooth::core::network
{
    /// Interface for packet send buffers
    /// \tparam Packet
    template<typename Protocol, typename Packet = typename Protocol::packet_type>
    class IPacketSendBuffer
    {
        public:
            virtual ~IPacketSendBuffer() = default;

            /// Returns a value indicating if a packet is currently being sent.
            /// \return true or false.
            virtual bool is_in_progress() = 0;

            /// Returns the start of the data to be sent.
            /// \return A pointer to the first byte of the data to send.
            virtual const uint8_t* get_data_to_send() = 0;

            /// Gets the number of bytes to be sent.
            /// \return The number of bytes remaining to be sent.
            virtual int get_remaining_data_length() = 0;

            /// Called when the specified amount of data has been sent.
            /// \param length The number of bytes that has been sent.
            virtual void data_has_been_sent(int length) = 0;

            /// Perpares the next packet to be sent.
            virtual void prepare_next_packet() = 0;

            /// Puts an item into the buffer to be sent.
            /// \return true if the item could be queued, otherwise false.
            virtual bool put(const Packet& item) = 0;

            /// Clears the buffer.
            virtual void clear() = 0;

            /// Returns an item indicating if the buffer is empty.
            /// \return true or false.
            virtual bool is_empty() = 0;
    };
}
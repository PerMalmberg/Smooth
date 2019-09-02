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
    /// Interface for a data packet that can be assembled
    template<typename Protocol, typename Packet = typename Protocol::packet_type>
    class IPacketAssembly
    {
        public:
            /// Must return the number of bytes the packet wants to fill
            /// its internal buffer, e.g. header, checksum etc. Returned
            /// value will differ depending on how much data already has been provided.
            /// \param packet The packet being assembled
            /// \return Number of bytes wanted
            virtual int get_wanted_amount(Packet& packet) = 0;

            /// Used by the underlying framework to notify the packet that {length} bytes
            /// has been written to the buffer pointed to by get_write_pos().
            /// During the call to this method the packet should do whatever it needs to
            /// evaluate if it needs more data or if it is complete.
            /// \param packet The packet being assembled
            /// \param length Number of bytes received
            virtual void data_received(Packet& packet, int length) = 0;

            /// Must return the current write position of the internal buffer.
            /// Must point to a buffer than can accept the number of bytes returned by
            /// get_wanted_amount().
            /// \param packet The packet being assembled
            /// \return Write position
            virtual uint8_t * get_write_pos(Packet& packet) = 0;

            /// Must return true when the packet has received all data it needs
            /// to fully assemble.
            /// \return true or false
            virtual bool is_complete(Packet& packet) const = 0;

            /// Must return true whenever the packet is unable to correctly assemble
            /// based on received data.
            /// \return true or false
            virtual bool is_error() = 0;

            /// Tells the protocol that the current working-package has been consumed.
            virtual void packet_consumed() = 0;

            /// Resets the protocol
            virtual void reset() = 0;

            virtual ~IPacketAssembly() = default;
    };
}

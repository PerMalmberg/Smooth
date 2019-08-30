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

namespace smooth::core::network
{
    /// Interface for packets that can be disassembled into a series of bytes
    class IPacketDisassembly
    {
        public:
            /// Must return the total amount of bytes to send
            /// \return Number of bytes to send
            virtual int get_send_length() = 0;

            /// Must return a pointer to the data to be sent.
            /// \return The read position
            virtual const uint8_t * get_data() = 0;

            virtual ~IPacketDisassembly() = default;
    };
}

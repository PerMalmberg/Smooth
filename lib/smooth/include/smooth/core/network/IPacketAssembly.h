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
            virtual uint8_t* get_write_pos(Packet& packet) = 0;

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

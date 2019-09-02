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

namespace smooth::core::network
{
    class LockedWritePos
    {
        public:
            explicit LockedWritePos(std::mutex& guard)
                    : lock(guard)
            {
            }

            LockedWritePos(LockedWritePos&& other) = default;

            LockedWritePos() = delete;

            LockedWritePos(const LockedWritePos&) = delete;

            LockedWritePos operator=(const LockedWritePos&) = delete;

            void set_pos(uint8_t* pos)
            { write_pos = pos; }

            explicit operator void*()
            {
                return write_pos;
            }

            explicit operator uint8_t*()
            {
                return write_pos;
            }
        private:
            std::unique_lock<std::mutex> lock;
            uint8_t* write_pos = nullptr;
    };

    /// Interface for packet receive buffers
    template<typename Protocol, typename Packet = typename Protocol::packet_type>
    class IPacketReceiveBuffer
    {
        public:
            virtual ~IPacketReceiveBuffer() = default;

            /// Returns a value stating if the buffer is full.
            /// \return true or false
            virtual bool is_full() = 0;

            /// Returns the amount of data the buffer wants in order to build its current package.
            /// \return The number of bytes wanted.
            virtual int amount_wanted() = 0;

            /// Gets the write position in the packet to which data should be written.
            /// \return
            virtual LockedWritePos get_write_pos() = 0;

            /// This method is called to notify the buffer that the specified amount of data has been written.
            /// \param length The number of bytes written.
            virtual void data_received(int length) = 0;

            /// Returns a value indicating if the current packet has been completed.
            /// \return true of false.
            virtual bool is_packet_complete() = 0;

            /// Gets the current packet. Don't call before is_packet_complete() returns true.
            /// \param target The instance which will be assigned the data.
            /// \return True if the packet could be received.
            virtual bool get(Packet& target) = 0;

            /// Clears the buffer
            virtual void clear() = 0;

            /// Returns a value indicating if a packet is being assembled.
            /// \return true or false.
            virtual bool is_in_progress() = 0;

            /// Prepares a new packet for assembly.
            virtual void prepare_new_packet() = 0;

            /// Returns a value indicating if an error has occurred during packet assembly, e.g. framing error.
            /// Normally this means that the connection should be closed and reconnected.
            virtual bool is_error() = 0;
    };
}

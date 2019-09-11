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

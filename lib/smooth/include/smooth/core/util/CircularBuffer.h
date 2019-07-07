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

namespace smooth::core::util
{
    /// Interface for a circular buffer.
    template<typename T>
    class ICircularBuffer
    {
        public:
            virtual ~ICircularBuffer() = default;

            /// Puts data onto the buffer
            virtual void put(const T& data) = 0;

            /// Gets data from the buffer
            /// \param t The item to put on the buffer
            /// \return true on success, false on failure.
            virtual bool get(T& t) = 0;

            /// Returns a value indicating if the buffer is empty.
            /// \return true or false
            virtual bool is_empty() = 0;

            /// Returns a value indicating if the buffer is full.
            /// \return true or false
            virtual bool is_full() = 0;

            /// Returns a value indicating number of available items.
            /// \return Number of items in the buffer.
            virtual int available_items() = 0;

            /// Returns a value indicating number of available empty slots.
            /// \return Number of items that can be put into the buffer.
            virtual int available_slots() = 0;

            /// Clears the buffer
            virtual void clear() = 0;

    };

    /// A circular buffer. Not thread-safe.
    /// \tparam T The type of item to hold
    /// \tparam Size Number of items to hold.
    template<typename T, int Size>
    class CircularBuffer
            : public ICircularBuffer<T>
    {
        public:
            CircularBuffer();

            virtual ~CircularBuffer() = default;

            void put(const T& data) override;

            bool get(T& d) override;

            bool is_empty() override
            {
                return count == 0;
            }

            bool is_full() override
            {
                return count == Size;
            }

            int available_items() override
            {
                return count;
            }

            int available_slots() override
            {
                return Size - count;
            }

            void clear() override
            {
                read_pos = 0;
                write_pos = 0;
                count = 0;
            }

            CircularBuffer(const CircularBuffer&) = delete;

            CircularBuffer& operator=(const CircularBuffer&) = delete;

        private:
            int next_pos(int current)
            {
                return (current + 1) % Size;
            }

            T buffer[static_cast<std::size_t>(Size)];
            int read_pos;
            int write_pos;
            int count;
    };


    template<typename T, int Size>
    CircularBuffer<T, Size>::CircularBuffer()
            :    buffer(),
                 read_pos(0),
                 write_pos(0),
                 count(0)
    {
    }

    template<typename T, int Size>
    void CircularBuffer<T, Size>::put(const T& data)
    {
        buffer[write_pos] = data;

        if (!is_full())
        {
            ++count;
        }

        // Overwrite data not yet read
        if (is_full() && read_pos == write_pos)
        {
            read_pos = next_pos(read_pos);
        }

        write_pos = next_pos(write_pos);
    }

    template<typename T, int Size>
    bool CircularBuffer<T, Size>::get(T& d)
    {
        bool res = false;

        if (!is_empty())
        {
            d = buffer[read_pos];
            read_pos = next_pos(read_pos);
            --count;

            res = true;
        }

        return res;
    }

}
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

#include <bitset>

namespace smooth::core::util
{
    class ByteSet
            : public std::bitset<8>
    {
        public:
            explicit ByteSet(uint8_t value)
                    : std::bitset<8>(value)
            {
            }

            operator uint8_t() const
            {
                return static_cast<uint8_t>(to_ulong());
            }

            /// Gets the value of the specified bits aligned to LSB.
            /// \param lsb Least Significant Bit, >= 0
            /// \param msb Most Significant Bit <= 7
            /// \return The resulting value
            uint8_t get_bits_as_byte(uint8_t lsb, uint8_t msb)
            {
                uint8_t res = 0;
                int ix = 0;

                for (uint8_t i = lsb; i <= msb; ++i)
                {
                    if (test(i))
                    {
                        res = static_cast<uint8_t>(res | (1 << ix));
                    }
                    ++ix;
                }

                return res;
            }
    };
}
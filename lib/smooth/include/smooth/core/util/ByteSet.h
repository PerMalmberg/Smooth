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

#include <bitset>

namespace smooth::core::util
{
    /// \brief ByteSet extends std::bitset<> with the ability to get specified bits as a byte.
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

            /// \brief Gets the value of the specified bits aligned to LSB.
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

//
// Created by permal on 8/13/17.
//

#pragma once

#include <bitset>

namespace smooth
{
    namespace core
    {
        namespace util
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
                                res |= 1 << ix;
                            }
                            ++ix;
                        }

                        return res;
                    }
            };
        }
    }
}
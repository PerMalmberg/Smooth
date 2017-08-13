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
                    ByteSet(uint8_t value)
                            : std::bitset<8>(value)
                    {
                    }

                    operator uint8_t() const
                    {
                        return static_cast<uint8_t>(to_ulong());
                    }
            };
        }
    }
}
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

#ifdef ESP_PLATFORM
    #include <machine/endian.h>
    #if _BYTE_ORDER == _LITTLE_ENDIAN
        #define SMOOTH_LITTLE_ENDIAN
    #else
        #define SMOOTH_BIG_ENDIAN
    #endif
#else
    #include <endian.h>
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        #define SMOOTH_LITTLE_ENDIAN
    #else
        #define SMOOTH_BIG_ENDIAN
    #endif
#endif

#if !defined(SMOOTH_LITTLE_ENDIAN) && !defined(SMOOTH_BIG_ENDIAN)
    #error "Could not determine endianness."
#endif

#include <algorithm>

namespace smooth::core::network
{
    template<typename T>
    constexpr T hton(T value) noexcept
    {
#ifdef SMOOTH_LITTLE_ENDIAN
        auto* ptr = reinterpret_cast<uint8_t*>(&value);
        std::reverse(ptr, ptr + sizeof(T));
#endif
        return value;
    }

    template<typename T>
    constexpr T ntoh(T value) noexcept
    {
        return hton(value);
    }
}

#undef LITTLE_ENDIAN
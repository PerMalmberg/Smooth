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

#include <endian.h>
#include <algorithm>

namespace smooth::core::network
{
    template<typename T>
    constexpr T hton(T value) noexcept
    {
#if __BYTE_ORDER == __LITTLE_ENDIAN
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
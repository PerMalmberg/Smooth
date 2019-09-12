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

#if defined(ESP_PLATFORM) || defined(__APPLE__)
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

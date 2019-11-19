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

#include "smooth/application/hash/sha.h"
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>

namespace smooth::application::hash
{
    std::array<uint8_t, 20> sha1(const uint8_t* data, std::size_t len)
    {
        std::array<uint8_t, 20> buff{};
        mbedtls_sha1_ret(data, len, buff.data());

        return buff;
    }

    std::array<uint8_t, 32> sha256(const uint8_t* data, std::size_t len)
    {
        std::array<uint8_t, 32> buff{};
        mbedtls_sha256_ret(data, len, buff.data(), 0);

        return buff;
    }
}

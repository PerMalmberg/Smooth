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

#include <memory>
#include <string>
#include "smooth/application/hash/base64.h"
#include <mbedtls/base64.h>

namespace smooth::application::hash::base64
{
    std::string encode(const uint8_t* data, std::size_t len)
    {
        auto buff_size = len * 2;
        std::size_t olen{ 0 };
        auto buff = std::make_unique<uint8_t[]>(buff_size);

        auto res = mbedtls_base64_encode(buff.get(), buff_size, &olen, data, len);

        while (res == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
        {
            buff = std::make_unique<uint8_t[]>(olen);
            res = mbedtls_base64_encode(buff.get(), buff_size, &olen, data, len);
        }

        std::string encoded{};

        if (res == 0)
        {
            encoded.assign(buff.get(), buff.get() + olen);

            // Ensure proper ending
            encoded.append("");
        }

        return encoded;
    }
}

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

#include <memory>
#include <string>
#include <smooth/application/hash/base64.h>
#include <mbedtls/base64.h>

namespace smooth::application::hash::base64
{
    std::string encode(const uint8_t* data, std::size_t len)
    {
        auto buff_size = len * 2;
        std::size_t olen{0};
        auto buff = std::make_unique<uint8_t[]>(buff_size);

        auto res = mbedtls_base64_encode(buff.get(), buff_size, &olen, data, len);
        while (res == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
        {
            buff = std::make_unique<uint8_t[]>(olen);
            res = mbedtls_base64_encode(buff.get(), buff_size, &olen, data, len);
        }

        std::string encoded{};

        if(res == 0)
        {
            encoded.assign(buff.get(), buff.get() + olen);
            // Ensure proper ending
            encoded.append("");
        }

        return encoded;
    }
}
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

#include "smooth/application/network/http/URLEncoding.h"
#include <array>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>

namespace smooth::application::network::http
{
    bool http::URLEncoding::decode(std::string& url)
    {
        return decode(url, url.begin(), url.end());
    }

    bool URLEncoding::decode(std::string& url, std::string::iterator begin, std::string::iterator end)
    {
        bool res = true;
        std::array<char, 3> hex{ 0, 0, 0 };

        // We know that the result string is going to be shorter or equal to the original,
        // so we'll do an in-place conversion to save on memory usage.

        auto write = begin;
        auto read = begin;

        for (; res && read != end; )
        {
            if (*read == '%')
            {
                if (std::distance(read, end) > 2)
                {
                    hex[0] = *(++read);
                    hex[1] = *(++read);
                    ++read;

                    auto a = std::isxdigit(static_cast<unsigned char>(hex[0]));
                    auto b = std::isxdigit(static_cast<unsigned char>(hex[1]));

                    res = a && b;

                    if (res)
                    {
                        try
                        {
                            *(write++) = static_cast<char>(std::stoul(hex.data(), nullptr, 16));
                        }
                        catch (...)
                        {
                            res = false;
                        }
                    }
                }
                else
                {
                    res = false;
                }
            }
            else
            {
                // Just keep going
                *write = *read;
                ++read;
                ++write;
            }
        }

        if (res)
        {
            *write = '\0';
            url.erase(write, end);
        }

        for (auto& c : url)
        {
            if (c == '+')
            {
                c = ' ';
            }
        }

        return res;
    }
}

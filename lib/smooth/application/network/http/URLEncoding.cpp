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

#include <smooth/application/network/http/URLEncoding.h>
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

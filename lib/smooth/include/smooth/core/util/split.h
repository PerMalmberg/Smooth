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

#include <vector>

namespace smooth::core::util
{
    template<typename Container>
    std::vector<Container> split(const Container& c, const Container& token)
    {
        std::vector<Container> res{};

        auto start = c.begin();
        auto pos = std::search(c.cbegin(), c.cend(), token.cbegin(), token.cend());

        while (pos != c.end())
        {
            res.emplace_back(Container(start, pos));

            start = pos + static_cast<long>(token.size());

            pos = std::search(start, c.cend(), token.cbegin(), token.cend());
        }

        if (std::distance(start, c.end()) > static_cast<long>(token.size()))
        {
            // Input data didn't contain or end with a token, add remains.
            res.emplace_back(Container(start, c.end()));
        }

        return res;
    }
}

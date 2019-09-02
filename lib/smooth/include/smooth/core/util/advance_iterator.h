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

#include <algorithm>

namespace smooth::core::util
{
    // Advances an iterator with the specified amount
    // unless it would put the iterator past the end.
    template<typename Iterator, typename Increment>
    bool advance(Iterator& current, const Iterator& end, Increment inc)
    {
        bool res = false;

        if (inc > 0)
        {
            auto dist = std::distance(current, end);
            auto to_move = std::min(static_cast<size_t>(dist), static_cast<size_t>(inc));
            res = inc == to_move;

            if (res)
            {
                std::advance(current, to_move);
            }
        }

        return res;
    }
}

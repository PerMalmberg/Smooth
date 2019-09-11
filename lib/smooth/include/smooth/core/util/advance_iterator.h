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

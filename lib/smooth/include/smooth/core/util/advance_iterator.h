//
// Created by permal on 8/14/17.
//

#pragma once

#include <algorithm>

namespace smooth
{
    namespace core
    {
        namespace util
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
            };
        }
    }
}

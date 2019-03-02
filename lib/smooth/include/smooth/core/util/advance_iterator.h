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
            // unless it would put the iterator past the end or the increment is < 0.
            template<typename T, typename Increment>
            bool advance(T& iterator, const T& end, Increment inc)
            {
                bool res = false;
                if(inc >= 0)
                {
                    Increment dist = static_cast<decltype(inc)>(std::distance(iterator, end));
                    Increment to_move = std::min(dist, inc);
                    res = inc == to_move;
                    if (res)
                    {
                        std::advance(iterator, to_move);
                    }
                }
                return res;
            };
        }
    }
}

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

#include <vector>

namespace smooth::core::util
{
    /// \brief Splits a container in two, at the point where the token is found, excluding the token itself.
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

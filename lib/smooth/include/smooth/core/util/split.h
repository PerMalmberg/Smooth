#pragma once

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
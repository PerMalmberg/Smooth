#include <utility>

#include <utility>

#pragma once

#include <algorithm>
#include <string>

namespace smooth::core::util
{
    inline std::string left_trim(std::string s, std::function<bool(char c)> filter = [](auto c) {return !std::isspace(c);})
    {
        s.erase(s.begin(),
                std::find_if(s.begin(), s.end(), std::move(filter)));
        return s;
    }

    inline std::string right_trim(std::string s, std::function<bool(char c)> filter = [](auto c) {return !std::isspace(c);})
    {
        auto erase_start = std::find_if(s.rbegin(), s.rend(), std::move(filter)).base();
        s.erase(erase_start, s.end());
        return s;
    }

    inline std::string trim(std::string s, const std::function<bool(char c)>& filter = [](auto c) {return !std::isspace(c);})
    {
        return right_trim(left_trim(std::move(s), filter), filter);
    }

    inline std::string to_lower_copy(std::string s)
    {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
        return s;
    }

    inline std::vector<std::string> split(const std::string& s, const std::string& token, bool trim_spaces = false)
    {
        std::vector<std::string> res{};

        auto start = s.begin();
        auto pos = std::search(s.cbegin(), s.cend(), token.cbegin(), token.cend());

        while (pos != s.end())
        {
            if (trim_spaces)
            {
                res.emplace_back(trim(std::string{start, pos}));
            }
            else
            {
                res.emplace_back(std::string{start, pos});
            }

            start = pos + static_cast<long>(token.size());

            pos = std::search(start, s.cend(), token.cbegin(), token.cend());
        }

        if(std::distance(start, s.end()) > static_cast<long>(token.size()))
        {
            // Input string didn't contain or end with a token, add remains.
            if (trim_spaces)
            {
                res.emplace_back(trim(std::string{start, s.end()}));
            }
            else
            {
                res.emplace_back(std::string{start, s.end()});
            }
        }

        return res;
    }
}
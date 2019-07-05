#pragma once


#include <utility>
#include <algorithm>
#include <string>
#include "split.h"

namespace smooth::core::string_util
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
        auto res = util::split<std::string>(s, token);

        if(trim_spaces)
        {
            for (auto& curr : res)
            {
                curr = trim(curr);
            }
        }

        return res;
    }
}
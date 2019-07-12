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

    void replace_all(std::string& s, const std::string& token, const std::string& replacement);
}
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

#include <utility>
#include <algorithm>
#include <string>
#include <functional>
#include "split.h"

namespace smooth::core::string_util
{
    constexpr auto is_not_space = [](auto c) { return !std::isspace(c); };

    inline std::string left_trim(std::string s, std::function<bool(char c)> filter = is_not_space)
    {
        s.erase(s.begin(),
                std::find_if(s.begin(), s.end(), std::move(filter)));

        return s;
    }

    inline std::string right_trim(std::string s, std::function<bool(char c)> filter = is_not_space)
    {
        auto erase_start = std::find_if(s.rbegin(), s.rend(), std::move(filter)).base();
        s.erase(erase_start, s.end());

        return s;
    }

    inline std::string trim(std::string s, const std::function<bool(char c)>& filter = is_not_space)
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

        if (trim_spaces)
        {
            for (auto& curr : res)
            {
                curr = trim(curr);
            }
        }

        return res;
    }

    bool icontains(const std::string& s, const std::string& to_find);

    void replace_all(std::string& s, const std::string& token, const std::string& replacement);

    int equals(const std::string& s, const std::string& s2);

    int iequals(const std::string& s, const std::string& s2);
}

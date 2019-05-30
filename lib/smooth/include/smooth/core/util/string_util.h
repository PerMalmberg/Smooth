#pragma once

#include <algorithm>
#include <string>

namespace smooth
{
    namespace core
    {
        namespace util
        {
            inline std::string& left_trim(std::string& s)
            {
                s.erase(s.begin(),
                        std::find_if(s.begin(), s.end(), [](std::string::value_type c) { return !std::isspace(c); }));
                return s;
            }

            inline std::string& right_trim(std::string& s)
            {
                auto erase_start = std::find_if(s.rbegin(), s.rend(),
                                                [](std::string::value_type c) { return !std::isspace(c); }).base();
                s.erase(erase_start, s.end());
                return s;
            }

            inline std::string& trim(std::string& s)
            {
                return right_trim(left_trim(s));
            }

            inline std::string to_lower_copy(std::string s)
            {
                std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
                return s;
            }
        }
    }
}
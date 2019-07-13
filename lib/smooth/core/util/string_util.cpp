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

#include <string>
#include <algorithm>

namespace smooth::core::string_util
{
    void replace_all(std::string& s, const std::string& token, const std::string& replacement)
    {
        auto pos = s.find(token);

        while (pos != std::string::npos)
        {
            s.replace(pos, token.length(), replacement);
            pos = s.find(token);
        }
    }

    bool icontains(const std::string& s, const std::string& to_find)
    {
        auto iequal = [](const unsigned char c, const unsigned char c2)
        {
            return std::tolower(c) == std::tolower(c2);
        };

        return std::search(s.begin(), s.end(), to_find.begin(), to_find.end(), iequal) != s.end();
    }

    bool equals(const std::string& s, const std::string& s2)
    {
        return s == s2;
    }

    bool iequals(const std::string& s, const std::string& s2)
    {
        return std::equal(s.begin(), s.end(), s2.begin(), s2.end(),
                          [](unsigned char c, unsigned char c2) {
                              return std::toupper(c) == std::toupper(c2);
                          });
    }
}
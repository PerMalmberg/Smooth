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

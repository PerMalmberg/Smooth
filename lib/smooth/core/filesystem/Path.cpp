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

#include "smooth/core/filesystem/Path.h"
#include <algorithm>

namespace smooth::core::filesystem
{
    void Path::append(const std::string& path)
    {
        if (p.empty())
        {
            p = path;
        }
        else
        {
            if (!equals_path_separator(*path.begin()))
            {
                p.append("/");
            }

            p.append(path);
        }

        remove_ending_separator();
    }

    bool Path::operator==(const Path& other) const
    {
        return p == other.p;
    }

    bool Path::operator==(const char* path) const
    {
        return *this == Path(path);
    }

    bool Path::operator!=(const Path& other) const
    {
        return !(*this == other);
    }

    bool Path::operator!=(const char* other) const
    {
        return !(*this == Path{ other });
    }

    bool Path::is_parent_of(const Path& child) const
    {
        auto res = false;

        if (is_absolute())
        {
            // Don't consider equal paths to be the parents of one another.
            if (p != child.p)
            {
                // If child begins with 'this', then 'this' is a parent of 'child'.
                // To prevent part of directory/file names to match (/a/w vs /a/what), prepend a
                // separator before comparing.
                auto child_path = child.p;
                child_path += separator;

                auto this_path = p;
                this_path += separator;

                res = child_path.find(this_path) == 0;
            }
        }

        return res;
    }

    Path Path::parent() const
    {
        Path res;

        if (p == separator)
        {
            res = ".";
        }
        else
        {
            auto end_of_parent = std::find_end(p.begin(), p.end(), separator.begin(), separator.end());

            if (end_of_parent == p.end())
            {
                res = ".";
            }
            else
            {
                res = Path(p.cbegin(), end_of_parent + 1);
            }
        }

        return res;
    }

    void Path::remove_ending_separator()
    {
        if (p != "/")
        {
            if (!p.empty() && *p.rbegin() == '/')
            {
                p = p.substr(0, p.size() - 1);
            }
        }
    }

    void Path::resolve()
    {
        std::vector<std::string> res{};

        auto absolute = is_absolute();

        if (p.find(dot_dot_token) != std::string::npos || p.find(dot_token) != std::string::npos)
        {
            auto prev = p.begin();
            auto pos = std::search(p.begin(), p.end(), separator.begin(), separator.end());

            if (pos != p.end())
            {
                while (pos != p.end())
                {
                    auto s = std::string(prev, pos);

                    if (!s.empty())
                    {
                        // Remove leading /
                        if (s.find('/') == 0)
                        {
                            s = { s.begin() + 1, s.end() };
                        }

                        // Skip paths referring to the current dir
                        if (s != ".")
                        {
                            res.emplace_back(s);
                        }
                    }

                    prev = pos;

                    pos = std::search(++pos, p.end(), separator.begin(), separator.end());
                }
            }

            if (prev + 1 != p.end())
            {
                res.emplace_back(prev + 1, p.end());
            }

            std::vector<std::string> final_path{};

            // Loop the split path, removing non-dots for each dot found
            for (auto& part : res)
            {
                if (contains_dots(part))
                {
                    if (!final_path.empty())
                    {
                        final_path.pop_back();
                    }
                }
                else
                {
                    final_path.push_back(std::move(part));
                }
            }

            p.clear();

            for (auto& s : final_path)
            {
                append(s);
            }

            if (absolute && !is_absolute())
            {
                p.insert(0, "/");
            }
        }

        // Treat empty paths as pointing to current directory
        if (p.empty())
        {
            p = ".";
        }
    }

    bool Path::contains_dots(const std::string& s)
    {
        return s == dot_dot_token;
    }

    bool Path::has_extension() const
    {
        return !extension().empty();
    }

    std::string Path::extension() const
    {
        auto last_separator = std::find_end(p.begin(), p.end(), separator.begin(), separator.end());
        auto last_dot = std::find_end(p.begin(), p.end(), dot_token.begin(), dot_token.end());

        std::string extension{};

        // The dot must be after the /, i.e. /foo/bar/file.ext
        if (last_dot > last_separator)
        {
            extension = { last_dot, p.end() };
        }

        return extension;
    }

    bool Path::has_parent() const
    {
        return parent() != Path{ "." };
    }
}

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

#include <smooth/core/filesystem/Path.h>
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

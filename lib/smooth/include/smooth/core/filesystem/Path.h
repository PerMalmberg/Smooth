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

#include <string>
#include <vector>

namespace smooth::core::filesystem
{
    class Path
    {
        public:
            Path() = default;

            Path(const Path&) = default;

            Path(Path&&) = default;

            Path& operator=(const Path&) = default;

            Path(const char* path)
                    : p(path)
            {
                remove_ending_separator();
                resolve();
            }

            Path(std::string::const_iterator beg, std::string::const_iterator end)
            {
                std::copy(beg, end, std::back_inserter(p));
                remove_ending_separator();
                resolve();
            }

            Path operator/(const char* path) const
            {
                Path tmp{*this};
                tmp /= path;
                return tmp;
            }

            Path operator/(const std::string& path) const
            {
                Path tmp{*this};
                tmp /= path.c_str();
                return tmp;
            }

            Path operator/(const Path& path) const
            {
                Path tmp{*this};
                tmp /= path;
                return tmp;
            }

            Path& operator/=(const char* path)
            {
                append(path);
                return *this;
            }

            Path& operator/=(const std::string& path)
            {
                append(path);
                return *this;
            }

            Path& operator/=(const Path& path)
            {
                append(path.p);
                return *this;
            }

            operator std::string() const
            {
                return p;
            }

            operator const char*() const
            {
                return p.c_str();
            }

            bool operator==(const Path& p) const;

            bool operator==(const char*) const;

            bool operator!=(const Path& p) const;

            bool operator!=(const char* p) const;

            [[nodiscard]] bool is_parent_of(const Path& child) const;

            [[nodiscard]] Path parent() const;

            [[nodiscard]] bool has_parent() const;

            [[nodiscard]] bool is_relative() const
            {
                return !is_absolute();
            }

            [[nodiscard]] bool is_absolute() const
            {
                return !p.empty() && p[0] == '/';
            }

            [[nodiscard]] bool empty() const
            {
                return p.empty();
            }

            [[nodiscard]] std::string extension() const;

            [[nodiscard]] bool has_extension() const;

            [[nodiscard]] std::string str() const
            {
                return p;
            }

        private:
            void append(const std::string& path);

            void remove_ending_separator();

            void resolve();

            bool static equals_path_separator(char c)
            {
                return c == '/' || c == '\\';
            }

            bool contains_dots(const std::string& s);

            std::string p;

            std::string separator = "/";
            std::string dot_token = ".";
            std::string dot_dot_token = "..";
    };
}

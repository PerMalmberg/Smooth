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
                Path tmp{ *this };
                tmp /= path;

                return tmp;
            }

            Path operator/(const std::string& path) const
            {
                Path tmp{ *this };
                tmp /= path.c_str();

                return tmp;
            }

            Path operator/(const Path& path) const
            {
                Path tmp{ *this };
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

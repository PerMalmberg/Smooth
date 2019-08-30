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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>
#include <smooth/core/filesystem/Path.h>

namespace smooth::core::filesystem
{
    class FileInfo
    {
        public:
            explicit FileInfo(const smooth::core::filesystem::Path& path)
                    : file_path(path)
            {
                struct stat s {};
                file_exists = stat(path, &s) == 0;
                regular_file = S_ISREG(s.st_mode);
                directory = S_ISDIR(s.st_mode);
                file_size = static_cast<std::size_t>(s.st_size);
                modified = s.st_mtime;
            }

            bool is_regular_file() const
            {
                return regular_file;
            }

            bool is_directory() const
            {
                return directory;
            }

            bool exists() const
            {
                return file_exists;
            }

            const time_t& last_modified() const
            {
                return modified;
            }

            const std::chrono::system_clock::time_point last_modified_point()
            {
                return std::chrono::system_clock::from_time_t(modified);
            }

            std::size_t size() const
            {
                return file_size;
            }

            const smooth::core::filesystem::Path& path() const
            {
                return file_path;
            }

        private:
            bool file_exists{ false };
            bool regular_file{ false };
            bool directory{ false };
            std::size_t file_size{ 0 };
            time_t modified{};
            const smooth::core::filesystem::Path file_path;
    };
}

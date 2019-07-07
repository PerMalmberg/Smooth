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

#include <vector>
#include <smooth/core/filesystem/Path.h>
#include <smooth/core/filesystem/FSLock.h>

namespace smooth::core::filesystem
{
    /// Utility class to read/write an entire file.
    class File
    {
        public:
            /// \param name The full path and name of the file.
            explicit File(std::string name);

            /// Reads the entire file contents into the vector.
            /// \param data The target container
            /// \return true on success, false on failure
            bool read(std::vector<uint8_t>& data) const;

            /// Reads a part of a file into the vector.
            /// \param data The target container
            /// \return true on success, false on failure
            static bool read(const smooth::core::filesystem::Path& path, std::vector<uint8_t>& data, std::size_t offset,
                             std::size_t length);

            /// Writes the entire vector to the file, overwriting any existing file.
            /// \param data The source container
            /// \return true on success, false on failure
            bool write(const std::string& data) const;

            /// Writes the provided data to the file, overwriting any existing file.
            /// \param data The data
            /// \param length The length
            /// \return true on success, false on failure
            bool write(const uint8_t* data, int length) const;

            /// Determines if the file exists
            bool exists() const;

            /// Determines if the file exists
            static bool exists(const char* name);

            /// Gets the size of the file, in bytes
            static uint_fast64_t file_size(const char* full_path);

            void remove();

        private:
            std::string name;
            FSLock lock{};
    };
}
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

#include <vector>
#include "smooth/core/filesystem/Path.h"
#include "smooth/core/filesystem/FSLock.h"
#include "smooth/core/logging/log.h"
#include <fstream>

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
            template<typename Container>
            bool read(Container& data) const
            {
                bool res = false;

                try
                {
                    std::fstream fs{ name, std::ios::binary | std::ios::in };

                    if (fs.is_open())
                    {
                        auto size = fs.seekg(0, std::ios::end).tellg();
                        fs.seekg(0, std::ios::beg);

                        // Reserve to ensure exact memory usage (i.e. no extra memory used)
                        data.reserve(static_cast<unsigned int>(size));

                        // Ensure that the vector thinks it has the number of elements we will write to it.
                        data.resize(static_cast<unsigned long>(size));

                        // std::vector has contiguous memory so we can write directly into it.
                        res = fs.read(reinterpret_cast<char*>(data.data()),
                            static_cast<std::streamsize>(size)).gcount() == size;
                    }
                }
                catch (std::exception& ex)
                {
                    using namespace smooth::core::logging;
                    Log::error("File", "Error reading file: {}", ex.what());
                }

                return res;
            }

            /// Reads a part of a file into the vector.
            /// \param data The target container
            /// \return true on success, false on failure
            static bool read(const smooth::core::filesystem::Path& path, std::vector<uint8_t>& data, std::size_t offset,
                             std::size_t length);

            /// Writes the entire vector to the file, overwriting any existing file.
            /// \param data The source container
            /// \return true on success, false on failure
            [[nodiscard]] bool write(const std::string& data) const;

            /// Writes the provided data to the file, overwriting any existing file.
            /// \param data The data
            /// \param length The length
            /// \return true on success, false on failure
            bool write(const uint8_t* data, int length) const;

            /// Determines if the file exists
            [[nodiscard]] bool exists() const;

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

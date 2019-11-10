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

#include <fstream>
#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "smooth/core/filesystem/File.h"

using namespace smooth::core::logging;

namespace smooth::core::filesystem
{
    smooth::core::filesystem::File::File(std::string name)
            : name(std::move(name))
    {
    }

    bool File::read(const smooth::core::filesystem::Path& path, std::vector<uint8_t>& data, std::size_t offset,
                    std::size_t length)
    {
        // Acquire a file lock
        FSLock lock;

        bool res = false;

        try
        {
            std::fstream fs(static_cast<const char*>(path), std::ios::binary | std::ios::in);

            if (fs.is_open())
            {
                fs.seekg(static_cast<int32_t>(offset), std::ios::beg);

                // Reserve to ensure exact memory usage (i.e. no extra memory used)
                data.reserve(static_cast<std::vector<uint8_t>::size_type>(length));

                // Ensure that the vector thinks it has the number of elements we will write to it.
                data.resize(static_cast<std::vector<uint8_t>::size_type>(length));

                // std::vector has contiguous memory so we can write directly into it.
                res = fs.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(length)).gcount() ==
                      static_cast<std::streamsize>(length);
            }
        }
        catch (std::exception& ex)
        {
            Log::error("File", "Error reading file: {}", ex.what());
        }

        return res;
    }

    bool File::write(const std::string& data) const
    {
        return write(reinterpret_cast<const uint8_t*>(data.data()), static_cast<int>(data.size()));
    }

    bool File::write(const uint8_t* data, int length) const
    {
        bool res = false;

        try
        {
            std::fstream fs{ name, std::ios::binary | std::ios::out | std::ios::trunc };

            if (fs.is_open())
            {
                fs.write(reinterpret_cast<const char*>(data), length);
                res = true;
            }
        }
        catch (std::exception& ex)
        {
            Log::error("File", "Error reading file: {}", ex.what());
        }

        return res;
    }

    bool File::exists() const
    {
        return exists(name.c_str());
    }

    bool File::exists(const char* full_path)
    {
        struct stat s {};

        return stat(full_path, &s) == 0;
    }

    uint_fast64_t File::file_size(const char* full_path)
    {
        struct stat s {};
        stat(full_path, &s);

        return static_cast<uint_fast64_t>(s.st_size);
    }

    void File::remove()
    {
        std::remove(name.c_str());
    }
}

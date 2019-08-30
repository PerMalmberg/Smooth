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

#include <fstream>
#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <smooth/core/filesystem/File.h>

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
            Log::error("File", Format("Error reading file: {1}", Str(ex.what())));
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
            Log::error("File", Format("Error reading file: {1}", Str(ex.what())));
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

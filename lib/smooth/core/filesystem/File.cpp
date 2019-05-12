#include <fstream>
#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <smooth/core/filesystem/File.h>
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

namespace smooth
{
    namespace core
    {
        namespace filesystem
        {
            smooth::core::filesystem::File::File(std::string name)
                    : name(std::move(name))
            {
            }

            bool File::read(std::vector<uint8_t>& data) const
            {
                bool res = false;

                try
                {
                    std::fstream fs{name, std::ios::binary | std::ios::in};
                    if (fs.is_open())
                    {
                        auto size = fs.seekg(0, std::ios::end).tellg();
                        fs.seekg(0, std::ios::beg);
                        // Reserve to ensure exact memory usage (i.e. no extra memory used)
                        data.reserve(static_cast<unsigned int>(size));
                        // Ensure that the vector thinks it has the number of elements we will write to it.
                        data.resize(static_cast<unsigned long>(size));
                        // std::vector has contiguous memory so we can write directly into it.
                        res = fs.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(size)).gcount() == size;
                    }
                }
                catch (std::exception& ex)
                {
                    Log::error("File", Format("Error reading file: {1}", Str(ex.what())));
                }

                return res;
            }

            bool File::read(const std::filesystem::path& path, std::vector<uint8_t>& data, int64_t offset, int64_t length)
            {
                bool res = false;

                try
                {
                    std::fstream fs{path, std::ios::binary | std::ios::in};
                    if (fs.is_open())
                    {
                        fs.seekg(offset, std::ios::beg);
                        // Reserve to ensure exact memory usage (i.e. no extra memory used)
                        data.reserve(static_cast<std::vector<uint8_t>::size_type>(length));
                        // Ensure that the vector thinks it has the number of elements we will write to it.
                        data.resize(static_cast<std::vector<uint8_t>::size_type>(length));
                        // std::vector has contiguous memory so we can write directly into it.
                        res = fs.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(length)).gcount() == static_cast<std::streamsize>(length);
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
                    std::fstream fs{name, std::ios::binary | std::ios::out | std::ios::trunc};
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
                struct stat s{};
                return stat(full_path, &s) == 0;                
            }

            uint_fast64_t File::file_size(const char* full_path)
            {
                struct stat s{};
                stat(full_path, &s);
                return static_cast<uint_fast64_t>(s.st_size);
            }
        }
    }
}
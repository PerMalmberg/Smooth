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
                struct stat s{};
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
            bool file_exists{false};
            bool regular_file{false};
            bool directory{false};
            std::size_t file_size{0};
            time_t modified{};
            const smooth::core::filesystem::Path file_path;
    };
}
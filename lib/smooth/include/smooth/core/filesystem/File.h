#pragma once


#include <vector>
#include <string>
#include <filesystem>

namespace smooth
{
    namespace core
    {
        namespace filesystem
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
                    static bool read(const std::filesystem::path& path, std::vector<uint8_t>& data, int64_t offset, int64_t length);

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
                private:
                    std::string name;
            };
        }
    }
}
#pragma once


#include <vector>
#include <string>

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

                    /// Writes the entire vector to the file, overwriting any existing file.
                    /// \param data The source container
                    /// \return true on success, false on failure
                    bool write(const std::string& data) const;

                    /// Writes the provided data to the file, overwriting any existing file.
                    /// \param data The data
                    /// \param length The length
                    /// \return true on success, false on failure
                    bool write(const uint8_t* data, size_t length) const ;
                private:
                    std::string name;
            };
        }
    }
}
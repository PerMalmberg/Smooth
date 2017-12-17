#pragma once

#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include <string>

namespace smooth
{
    namespace core
    {
        namespace filesystem
        {
            /// The Filesystem class handles initialization of the underlying file
            /// system and wear leveling drivers for flash storage.
            /// \param root The root of the paths used for files, e.g. /my_path
            /// \param partition_name The name of the partition, as specified in the partition table
            ///         flashed to the system of type 'data' and subtype 'fat', e.g:
            ///         # Name,       Type, SubType, Offset,  Size
            ///         ...
            ///         app_storage,  data, fat,     ,        0x2000
            ///         ...
            /// \param max_file_count Maximum number of files.
            /// \param format_on_mount_failure If true, the partition will be formatted if mount fails.
            class Filesystem
            {
                public:
                    explicit Filesystem(const char* root, const char* partition_name, int max_file_count, bool format_on_mount_failure);
                    bool mount();
                    void unmount();
                private:
                    const char* tag = "Filesystem";
                    std::string root;
                    std::string partition_name;
                    int max_file_count;
                    bool format_on_mount_failure;
                    wl_handle_t wl_instance = WL_INVALID_HANDLE;
            };
        }
    }
}
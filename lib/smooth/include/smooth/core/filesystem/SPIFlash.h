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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <esp_vfs.h>
#include <esp_vfs_fat.h>
#include <esp_system.h>
#pragma GCC diagnostic pop

#include <string>
#include "MountPoint.h"

namespace smooth::core::filesystem
{
    /// The SPIFlash class handles initialization of the underlying file
    /// system and wear leveling drivers for internal SPI flash storage.
    ///
    /// There is a minimum size the partition can be
    ///
    /// \param root The root of the paths used for files, e.g. /my_path
    /// \param partition_name The name of the partition, as specified in the partition table
    ///         flashed to the system of type 'data' and subtype 'fat', e.g:
    ///         # Name,       Type, SubType, Offset,  Size
    ///         ...
    ///         app_storage,  data, fat,     ,        0x2000
    ///         ...
    /// \param max_file_count Maximum number of files.
    /// \param format_on_mount_failure If true, the partition will be formatted if mount fails.
    class SPIFlash
    {
        public:
            explicit SPIFlash(const FlashMount& mount_point, const char* partition_name, int max_file_count,
                              bool format_on_mount_failure);

            bool mount();

            void unmount();

        private:
            const char* tag = "SPIFlash";
            std::string root;
            std::string partition_name;
            int max_file_count;
            bool format_on_mount_failure;
            wl_handle_t wl_instance = WL_INVALID_HANDLE;
    };
}

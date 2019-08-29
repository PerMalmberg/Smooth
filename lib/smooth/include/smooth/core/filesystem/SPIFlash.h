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

#include "esp_vfs.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

#include "esp_vfs_fat.h"

#pragma GCC diagnostic pop

#include <esp_system.h>
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
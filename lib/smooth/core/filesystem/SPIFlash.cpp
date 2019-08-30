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

#include <smooth/core/filesystem/SPIFlash.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/filesystem/FSLock.h>

using namespace smooth::core::logging;

namespace smooth::core::filesystem
{
    SPIFlash::SPIFlash(const FlashMount& mount_point, const char* partition_name, int max_file_count,
                       bool format_on_mount_failure)
            : root(*mount_point),
              partition_name(partition_name),
              max_file_count(max_file_count),
              format_on_mount_failure(format_on_mount_failure)
    {
        FSLock::init(max_file_count);
    }

    bool SPIFlash::mount()
    {
        esp_vfs_fat_mount_config_t mount_config{};
        mount_config.format_if_mount_failed = format_on_mount_failure;
        mount_config.max_files = max_file_count;

        {
            Format msg("Mounting partition {1} in root {2}, with max file count {3}...",
                       Str(partition_name),
                       Str(root),
                       Int32(max_file_count));
            Log::info(tag, msg);
        }

        auto res = esp_vfs_fat_spiflash_mount(root.c_str(),
                                              partition_name.c_str(),
                                              &mount_config,
                                              &wl_instance);

        Format msg("Result: {1}", Str(res == ESP_OK ? "OK" : "FAIL"));

        // Getting mount failures? Make sure that the partition is large enough.
        // https://www.esp32.com/viewtopic.php?f=13&t=1897&p=8909&hilit=f_mkfs+failed+14#p8911
        // Reduce wear leveling sector size if you can't increase partition size.

        if (res == ESP_OK)
        {
            Log::info(tag, msg);
        }
        else
        {
            Log::error(tag, msg);
        }

        return res == ESP_OK;
    }

    void SPIFlash::unmount()
    {
        if (wl_instance != WL_INVALID_HANDLE)
        {
            {
                Format msg("Unmounting partition {1} in root {2}.",
                           Str(partition_name),
                           Str(root));
                Log::info(tag, msg);
            }

            auto res = esp_vfs_fat_spiflash_unmount(root.c_str(), wl_instance);

            Format msg("Result: {1}", Str(res == ESP_OK ? "OK" : "FAIL"));

            if (res == ESP_OK)
            {
                Log::info(tag, msg);
            }
            else
            {
                Log::error(tag, msg);
            }
        }
    }
}

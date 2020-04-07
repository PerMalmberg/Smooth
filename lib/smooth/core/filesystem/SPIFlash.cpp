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

#include "smooth/core/filesystem/SPIFlash.h"
#include "smooth/core/logging/log.h"
#include "smooth/core/filesystem/FSLock.h"

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
        FSLock::set_limit(max_file_count);
    }

    bool SPIFlash::mount()
    {
        esp_vfs_fat_mount_config_t mount_config{};
        mount_config.format_if_mount_failed = format_on_mount_failure;
        mount_config.max_files = max_file_count;

        Log::info(tag, "Mounting partition {} in root {}, with max file count {}",
                           partition_name, root,
                           max_file_count);

        auto res = esp_vfs_fat_spiflash_mount(root.c_str(),
                                              partition_name.c_str(),
                                              &mount_config,
                                              &wl_instance);

        // Getting mount failures? Make sure that the partition is large enough.
        // https://www.esp32.com/viewtopic.php?f=13&t=1897&p=8909&hilit=f_mkfs+failed+14#p8911
        // Reduce wear leveling sector size if you can't increase partition size.

        if (res == ESP_OK)
        {
            Log::info(tag, "Result: OK");
        }
        else
        {
            Log::error(tag, "Result: FAILE");
        }

        return res == ESP_OK;
    }

    void SPIFlash::unmount()
    {
        if (wl_instance != WL_INVALID_HANDLE)
        {
            {
                Log::info(tag, "Unmounting partition {} in root {}.", partition_name, root);
            }

            auto res = esp_vfs_fat_spiflash_unmount(root.c_str(), wl_instance);

            if (res == ESP_OK)
            {
                Log::info(tag, "Result: OK");
            }
            else
            {
                Log::error(tag, "Result: FAIL");
            }
        }
    }
}

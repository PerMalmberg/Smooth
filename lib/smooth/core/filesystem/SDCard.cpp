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

#include "smooth/core/logging/log.h"
#include "smooth/core/filesystem/SDCard.h"
#include "smooth/core/filesystem/FSLock.h"

using namespace smooth::core::logging;

namespace smooth::core::filesystem
{
    bool SDCard::do_common_initialization(const MountPoint& mount_point,
                                          int max_file_count,
                                          bool format_on_mount_failure,
                                          void* slot_config)
    {
        esp_vfs_fat_sdmmc_mount_config_t mount_config{};
        mount_config.format_if_mount_failed = format_on_mount_failure;
        mount_config.max_files = max_file_count;

        //mount_config.allocation_unit_size = 16 * 1024;

        auto mount_result = esp_vfs_fat_sdmmc_mount((*mount_point).c_str(), &host, slot_config, &mount_config, &card);

        initialized = mount_result == ESP_OK;

        if (initialized)
        {
            Log::info("SPISDCard", "SD Card initialized");
            sdmmc_card_print_info(stdout, card);
        }
        else
        {
            if (mount_result == ESP_FAIL)
            {
                Log::error("SPISDCard", "Failed to mount the file system.");
            }
            else
            {
                Log::error("SPISDCard", "Failed to initialize SD Card.");
            }
        }

        FSLock::set_limit(max_file_count);

        return initialized;
    }

    bool SDCard::deinit()
    {
        auto res = ESP_OK;

        if (is_initialized())
        {
            res = esp_vfs_fat_sdmmc_unmount();
            initialized = false;
        }

        return res == ESP_OK;
    }
}

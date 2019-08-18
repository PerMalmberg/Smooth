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

#include <smooth/core/logging/log.h>
#include <smooth/core/filesystem/SDCard.h>
#include <smooth/core/filesystem/FSLock.h>


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

        auto mount_result = esp_vfs_fat_sdmmc_mount(*mount_point, &host, slot_config, &mount_config, &card);

        initialized = mount_result == ESP_OK;
        if (initialized)
        {
            Log::info("SPISDCard", Format("SD Card initialized"));
            sdmmc_card_print_info(stdout, card);
        }
        else
        {
            if (mount_result == ESP_FAIL)
            {
                Log::error("SPISDCard", Format("Failed to mount the file system."));
            }
            else
            {
                Log::error("SPISDCard", Format("Failed to initialize SD Card."));
            }
        }

        FSLock::init(max_file_count);

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
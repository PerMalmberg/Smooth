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

using namespace smooth::core::logging;

namespace smooth::core::filesystem
{
    static constexpr const char* log_tag = "SDCard";

    bool SDCard::do_common_error_checking(esp_err_t result)
    {
        initialized = result == ESP_OK;

        if (initialized)
        {
            Log::info(log_tag, "Storage Card initialized");
            sdmmc_card_print_info(stdout, card);
        }
        else
        {
            if (result == ESP_FAIL)
            {
                Log::error(log_tag, "Failed to mount the file system.");
            }
            else
            {
                Log::error(log_tag, "Failed to initialize storage card.");
            }
        }

        return initialized;
    }

    bool SDCard::deinit()
    {
        auto res = ESP_OK;

        if (is_initialized())
        {
            res = esp_vfs_fat_sdcard_unmount(SDCardMount::instance().mount_point().str().data(), card);
            initialized = false;

            if (res != ESP_OK)
            {
                Log::error(log_tag, "SDCard unmount failed");
            }
        }

        return res == ESP_OK;
    }
}

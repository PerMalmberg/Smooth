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

#include "MountPoint.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <sdmmc_cmd.h>
#include <esp_vfs_fat.h>
#include <esp_vfs.h>
#pragma GCC diagnostic pop

namespace smooth::core::filesystem
{
    class SDCard
    {
        public:
            virtual ~SDCard()
            {
                deinit();
            }

            virtual bool init(const SDCardMount& mount, bool format_on_mount_failure, int max_file_count) = 0;

            [[nodiscard]] virtual bool is_initialized() const
            {
                return initialized;
            }

            virtual bool deinit();

        protected:
            bool do_common_initialization(const MountPoint& mount_point,
                                          int max_file_count,
                                          bool format_on_mount_failure,
                                          void* slot_config);

            sdmmc_host_t host{};
            sdmmc_card_t* card{};
            bool initialized{};
        private:
    };
}

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

#include "MountPoint.h"

#ifndef ESP_PLATFORM
#include "mock/SDCard.h"
#else

#include <sdmmc_cmd.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"

#include <esp_vfs_fat.h>

#pragma GCC diagnostic pop

#include <esp_vfs.h>

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
#endif

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

#include <smooth/core/filesystem/MountPoint.h>
#include <smooth/core/filesystem/FSLock.h>
#include <smooth/core/filesystem/filesystem.h>

namespace smooth::core::filesystem
{
    class SPIFlash
    {
        public:
            SPIFlash(const FlashMount& mount_point,
                     const char* /*partition_name*/,
                     int max_file_count,
                     bool /*format_on_mount_failure*/)
            {
                initialized = is_directory(mount_point.mount_point());

                if(!initialized)
                {
                    Log::error("Mock-SPIFlash",
                               mount_point.mount_point().str() + " does not exist, please create it before running.");
                }
                FSLock::init(max_file_count);
            }

            bool mount()
            {
                return initialized;
            }

            void unmount() {}

        private:
            bool initialized{};
    };
}
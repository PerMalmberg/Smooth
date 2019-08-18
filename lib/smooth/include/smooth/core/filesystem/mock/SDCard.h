// Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
// Copyright (C) 2019 Per Malmberg (https://github.com/PerMalmberg)
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
#include <smooth/core/logging/log.h>
#include <smooth/core/filesystem/filesystem.h>

namespace smooth::core::filesystem
{
    class SDCard
    {
        public:
            virtual ~SDCard() = default;

            virtual bool init(const SDCardMount& mount, bool /*format_on_mount_failure*/, int max_file_count)
            {
                initialized = is_directory(mount.mount_point());

                if (!initialized)
                {
                    logging::Log::error("Mock-SDCard", mount.mount_point().str() +
                                                       " does not exist, please create it before running.");
                }
                FSLock::init(max_file_count);

                return initialized;
            };

            [[nodiscard]] virtual bool is_initialized() const
            {
                return initialized;
            }

            void deinit() {};

        protected:
            bool initialized{};
        private:
    };
}
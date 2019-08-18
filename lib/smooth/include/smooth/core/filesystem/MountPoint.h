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

#include <utility>
#include "Path.h"

#ifdef ESP_PLATFORM
static smooth::core::filesystem::Path FlashMount{"/flash}";
static smooth::core::filesystem::Path SDMount{"/sdcard"};
#else
// Place files in home folder on Linux.
static smooth::core::filesystem::Path FMount = smooth::core::filesystem::Path{getenv("HOME")} / "smooth-data" / "flash";
static smooth::core::filesystem::Path SDMount = smooth::core::filesystem::Path{getenv("HOME")} / "smooth-data" / "sdcard";
#endif


namespace smooth::core::filesystem
{
    /**
     * \brief The MountPoint class defines a mount point for the file system.
     *
     * The idea is that since only one instance of the respective subclasses
     * exists, they provide a known way to get the used mount point
     * throughout the life of the application.
     *
     * If you want to use another mount point other than the standard, be sure to call
     * ::instance() with an root path before mounting your file system.
     */

    class MountPoint
    {
        public:
            [[nodiscard]] Path mount_point() const noexcept
            {
                return point;
            }

            const char* operator*() const
            {
                return point.str().c_str();
            }

        protected:
            explicit MountPoint(Path p)
                    : point(std::move(p))
            {

            }

        private:
            const Path point;
    };

    class SDCardMount
            : public MountPoint
    {
        public:
            static const SDCardMount& instance(Path mount_point = SDMount) noexcept
            {
                static SDCardMount sdcm{std::move(mount_point)};
                return sdcm;
            }

        private:
            explicit SDCardMount(Path mount_point)
                    : MountPoint(std::move(mount_point))
            {}
    };

    class FlashMount
            : public MountPoint
    {
        public:
            static const FlashMount& instance(Path mount_point = FMount) noexcept
            {
                static FlashMount sdcm{std::move(mount_point)};
                return sdcm;
            }

        private:
            explicit FlashMount(Path mount_point)
                    : MountPoint(std::move(mount_point))
            {}
    };
}
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

#include <utility>
#include "Path.h"

#ifdef ESP_PLATFORM
static smooth::core::filesystem::Path FMount{ "/flash" };
static smooth::core::filesystem::Path SDMount{ "/sdcard" };
#else

#include <sys/stat.h>

// Place files in home folder on Linux.
static smooth::core::filesystem::Path FMount =
    smooth::core::filesystem::Path{ getenv("HOME") } / "smooth-data";
static smooth::core::filesystem::Path SDMount =
    smooth::core::filesystem::Path{ getenv("HOME") } / "smooth-data";
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

            std::string operator*() const
            {
                return point.str();
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
                static SDCardMount sdcm{ std::move(mount_point) };

                return sdcm;
            }

        private:
            explicit SDCardMount(Path mount_point)
                    : MountPoint(std::move(mount_point))
            {
#ifndef ESP_PLATFORM
                const auto& parent = SDMount.parent().str();
                mkdir(parent.c_str(), S_IWUSR | S_IRUSR | S_IXUSR);
#endif
            }
    };

    class FlashMount
        : public MountPoint
    {
        public:
            static const FlashMount& instance(Path mount_point = FMount) noexcept
            {
                static FlashMount sdcm{ std::move(mount_point) };

                return sdcm;
            }

        private:
            explicit FlashMount(Path mount_point)
                    : MountPoint(std::move(mount_point))
            {
#ifndef ESP_PLATFORM
                const auto& parent = FMount.parent().str();
                mkdir(parent.c_str(), S_IWUSR | S_IRUSR | S_IXUSR);
#endif
            }
    };
}

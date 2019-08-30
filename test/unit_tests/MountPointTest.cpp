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

#include "catch.hpp"
#include <smooth/core/filesystem/MountPoint.h>

using namespace smooth::core::filesystem;

SCENARIO("SDCardMount")
{
    const auto& instance = SDCardMount::instance("/first");
    REQUIRE(instance.mount_point() == "/first");
    WHEN("Calling factory method second time, mount point does not change")
    {
        const auto& second = SDCardMount::instance("/second");
        REQUIRE(second.mount_point() == "/first");
    }
}

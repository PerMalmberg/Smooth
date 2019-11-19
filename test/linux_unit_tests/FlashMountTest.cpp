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

#include <catch2/catch.hpp>
#include "smooth/core/filesystem/MountPoint.h"

using namespace smooth::core::filesystem;

SCENARIO("FlashMount")
{
    const auto& instance = FlashMount::instance("/first");
    REQUIRE(instance.mount_point() == "/first");
    WHEN("Calling factory method second time, mount point does not change")
    {
        const auto& second = FlashMount::instance("/second");
        REQUIRE(second.mount_point() == "/first");
    }
}

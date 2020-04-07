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
#include "smooth/core/json/JsonFile.h"
#include "smooth/core/filesystem/MountPoint.h"
#include "smooth/core/util/json_util.h"

using namespace std;
using namespace smooth::core::filesystem;
using namespace smooth::core::json;
using namespace smooth::core::json_util;

SCENARIO("JsonFile Test")
{
    FSLock::set_limit(5);

    {
        JsonFile jf{ Path{ "test_data" } / "jsonfile_test.json" };
        jf.value()["Foo"] = 1;
        REQUIRE(jf.save());
    }
    {
        JsonFile jf{ Path{ "test_data" } / "jsonfile_test.json" };
        REQUIRE(jf.value()["Foo"] == 1);
    }
    {
        nlohmann::json j{};
        REQUIRE(default_value(j, "foo", "asdf") == "asdf");
        REQUIRE(default_value(j, "bar", true));
    }
}

SCENARIO("Json default value test")
{
    nlohmann::json j{};
    REQUIRE(default_value(j, "foo", "asdf") == "asdf");
    REQUIRE(default_value(j, "bar", true));
}

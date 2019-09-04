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

#include "catch.hpp"
#include <smooth/core/json/JsonFile.h>
#include <smooth/core/filesystem/MountPoint.h>
#include <smooth/core/util/json_util.h>

using namespace std;
using namespace smooth::core::filesystem;
using namespace smooth::core::json;
using namespace smooth::core::json_util;

SCENARIO("JsonFile Test")
{
    FSLock::set_limit(5);

    {
        JsonFile jf{ SDCardMount::instance().mount_point() / "jsonfile_test.json" };
        jf.value()["Foo"] = 1;
        REQUIRE(jf.save());
    }
    {
        JsonFile jf{ SDCardMount::instance().mount_point() / "jsonfile_test.json" };
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

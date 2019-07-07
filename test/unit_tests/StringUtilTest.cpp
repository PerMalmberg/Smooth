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

#include <catch.hpp>

#include <smooth/core/util/string_util.h>

using namespace smooth::core::string_util;

SCENARIO("Trimming texts")
{
    REQUIRE(left_trim("   ASDF   ") == "ASDF   ");
    REQUIRE(right_trim("   ASDF   ") == "   ASDF");
    REQUIRE(trim("   ASDF   ") == "ASDF");
}

SCENARIO("Trimming with custom character")
{
    auto filter = [](char c) { return c != '|'; };
    REQUIRE(left_trim("|| ASDF  ||", filter) == " ASDF  ||");
    REQUIRE(right_trim("|| ASDF  ||", filter) == "|| ASDF  ");
    REQUIRE(trim("|| ASDF  ||", filter) == " ASDF  ");
}

SCENARIO("Split")
{
    std::string s = "asdfjklö";
    std::vector<std::string> res = split(s, "f");
    REQUIRE(res[0] == "asd");
    REQUIRE(res[1] == "jklö");
    REQUIRE(res.size() == 2);

    // When no toke is found, place all in first result.
    res = split(s, "foo");
    REQUIRE(res[0] == s);
    REQUIRE(res.size() == 1);

    std::string key_value = "     key  =     value   ";
    res = split(key_value, "=", true);
    REQUIRE(res[0] == "key");
    REQUIRE(res[1] == "value");
    REQUIRE(res.size() == 2);

    // Remains placed in last entry of vector
    std::string key_value2 = "key=value;key";
    res = split(key_value2, ";");
    REQUIRE(res[0] == "key=value");
    REQUIRE(res[1] == "key");
    REQUIRE(res.size() == 2);


}
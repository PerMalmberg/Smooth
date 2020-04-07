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

#include "smooth/core/util/string_util.h"

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

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
#include <catch.hpp>

#include <smooth/core/util/string_util.h>

using namespace smooth::core::util;

SCENARIO("Trimming texts")
{
    REQUIRE(left_trim("   ASDF   ") == "ASDF   ");
    REQUIRE(right_trim("   ASDF   ") == "   ASDF");
    REQUIRE(trim("   ASDF   ") == "ASDF");
}

SCENARIO("Trimming with custom character")
{
    auto filter = [](char c){ return c != '|';};
    REQUIRE(left_trim("|| ASDF  ||", filter) == " ASDF  ||");
    REQUIRE(right_trim("|| ASDF  ||", filter) == "|| ASDF  ");
    REQUIRE(trim("|| ASDF  ||", filter) == " ASDF  ");
}
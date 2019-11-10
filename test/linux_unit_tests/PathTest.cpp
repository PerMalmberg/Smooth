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
#include <array>
#include <unordered_map>

#include "smooth/core/filesystem/Path.h"

using namespace smooth::core::filesystem;

SCENARIO("Path")
{
    GIVEN("An empty path")
    {
        Path p;

        THEN("Path is empty")
        {
            REQUIRE(p.empty());
        }
        WHEN("Appending")
        {
            auto a = p / "foo";
            REQUIRE(a == "foo");
        }
    }
}

SCENARIO("Relative")
{
    GIVEN("A relative path")
    {
        Path p("a/b");
        REQUIRE(p.is_relative());
        REQUIRE(!p.is_absolute());
    }

    GIVEN("A full path")
    {
        Path p("/a/b/c");
        REQUIRE(!p.is_relative());
        REQUIRE(p.is_absolute());
    }
}

SCENARIO("Parent path")
{
    GIVEN("A path with some depth")
    {
        Path p("/a/b/c/d/");

        WHEN("Getting parent path")
        {
            Path parent = p.parent();
            THEN("Parent path matches")
            {
                REQUIRE(parent == "/a/b/c");
                REQUIRE(parent.has_parent());
            }
        }
    }
    GIVEN("A relative path with no depth")
    {
        Path p{ "a" };
        WHEN("Getting parent path")
        {
            auto parent = p.parent();
            THEN("Parent is current dir")
            {
                REQUIRE(parent == ".");
                REQUIRE_FALSE(p.has_parent());
            }
        }
    }
    GIVEN("A relative path with no depth and ending slash")
    {
        Path p{ "a/" };
        WHEN("Getting parent path")
        {
            auto parent = p.parent();
            THEN("Parent is current dir")
            {
                REQUIRE(parent == ".");
            }
        }
    }

    GIVEN("An absolute path with no depth")
    {
        Path p{ "/a" };
        WHEN("Getting parent path")
        {
            auto parent = p.parent();
            THEN("Parent is root dir")
            {
                REQUIRE(parent == "/");
                REQUIRE_FALSE(parent.has_parent());
            }
        }
    }
}

SCENARIO("Parent of")
{
    GIVEN("A path")
    {
        REQUIRE(Path{ "/a" }.is_parent_of(Path{ "/a/b/c/d" }));
        REQUIRE(Path{ "/a/b/c" }.is_parent_of(Path{ "/a/b/c/d" }));

        REQUIRE_FALSE(Path{ "/q" }.is_parent_of(Path{ "/q" }));
        REQUIRE_FALSE(Path{ "/q/w" }.is_parent_of(Path{ "/q" }));
        REQUIRE_FALSE(Path{ "/a/w" }.is_parent_of("/a/what"));
    }
}

SCENARIO("Breaking out of path")
{
    GIVEN("Paths with ..")
    {
        REQUIRE(Path{ "/a/b/../c" } == "/a/c");
        REQUIRE(Path{ "/../b/../c" } == "/c");
        REQUIRE(Path{ "/../b/../c/../.." } == "/");
        REQUIRE(Path{ "b/.." } == ".");
        REQUIRE(Path{ "/b/.." } == "/");
        REQUIRE(Path{ "a/b/c/d/e/../../../../.." } == ".");
        REQUIRE(Path{ "a/b/c/d/e/../x/../../.." } == "a/b");
        REQUIRE(Path{ "/a/b/c/d/e/../x/../../.." } == "/a/b");

        REQUIRE(Path{ "/a/./c/" } == "/a/c");
        REQUIRE(Path{ "/./././c" } == "/c");

        REQUIRE(Path{ "/a/b/../c" }.is_parent_of("/a/c/d"));

        REQUIRE_FALSE(Path{ "/www_root" }.is_parent_of("/www_root/../cgi-bin"));
    }
}

SCENARIO("Path with spaces")
{
    GIVEN("Paths with spaces")
    {
        REQUIRE(Path{ "/a/b/.. /c" } == "/a/b/.. /c");
        REQUIRE(Path{ "/ / / /c" } == "/ / / /c");
        REQUIRE(Path{ "/ / /../c" } == "/ / /../c");
        REQUIRE(Path{ "/ /x/../c" } == "/ /c");
    }
}

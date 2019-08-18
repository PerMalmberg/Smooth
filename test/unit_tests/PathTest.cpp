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
#include <array>
#include <unordered_map>

#include <smooth/core/filesystem/Path.h>

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
        Path p{"a"};
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
        Path p{"a/"};
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
        Path p{"/a"};
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
        REQUIRE(Path{"/a"}.is_parent_of(Path{"/a/b/c/d"}));
        REQUIRE(Path{"/a/b/c"}.is_parent_of(Path{"/a/b/c/d"}));

        REQUIRE_FALSE(Path{"/q"}.is_parent_of(Path{"/q"}));
        REQUIRE_FALSE(Path{"/q/w"}.is_parent_of(Path{"/q"}));
        REQUIRE_FALSE(Path{"/a/w"}.is_parent_of("/a/what"));
    }
}

SCENARIO("Breaking out of path")
{
    GIVEN("Paths with ..")
    {
        REQUIRE(Path{"/a/b/../c"} == "/a/c");
        REQUIRE(Path{"/../b/../c"} == "/c");
        REQUIRE(Path{"/../b/../c/../.."} == "/");
        REQUIRE(Path{"b/.."} == ".");
        REQUIRE(Path{"/b/.."} == "/");
        REQUIRE(Path{"a/b/c/d/e/../../../../.."} == ".");
        REQUIRE(Path{"a/b/c/d/e/../x/../../.."} == "a/b");
        REQUIRE(Path{"/a/b/c/d/e/../x/../../.."} == "/a/b");


        REQUIRE(Path{"/a/./c/"} == "/a/c");
        REQUIRE(Path{"/./././c"} == "/c");

        REQUIRE(Path{"/a/b/../c"}.is_parent_of("/a/c/d"));

        REQUIRE_FALSE(Path{"/www_root"}.is_parent_of("/www_root/../cgi-bin"));
    }
}

SCENARIO("Path with spaces")
{
    GIVEN("Paths with spaces")
    {
        REQUIRE(Path{"/a/b/.. /c"} == "/a/b/.. /c");
        REQUIRE(Path{"/ / / /c"} == "/ / / /c");
        REQUIRE(Path{"/ / /../c"} == "/ / /../c");
        REQUIRE(Path{"/ /x/../c"} == "/ /c");
    }
}

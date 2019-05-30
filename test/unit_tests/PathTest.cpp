#include <catch.hpp>
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

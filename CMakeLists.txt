# This CMakeLists.txt is meant for use when developing the smooth framework.
# If you are attepmting to build smooth as a library, you'll want to use the
# the one in the lib/smooth-folder instead.

cmake_minimum_required(VERSION 3.10)

set(ACTIVE_TEST hello_world)

if (${ESP_PLATFORM})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    set(EXTRA_COMPONENT_DIRS test lib)
    set(COMPONENT_REQUIRES hello_world)
    add_definitions(-D_GLIBCXX_USE_C99) # for std::to_string (until using xtensa-gcc 8)
    project(smooth-test)
else ()
    add_subdirectory(lib/smooth)
    add_subdirectory(test/hello_world)
    add_subdirectory(test/main)
endif ()
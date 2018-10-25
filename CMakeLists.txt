# This CMakeLists.txt is meant for use when developing the smooth framework.
# If you are attepmting to build smooth as a library, you'll want to use the
# the one in the lib/smooth-folder instead.

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)

# Select the test project, then build target "main"

set(CURRENT_PROJECT_NAME mqtt)



set(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}/test/${CURRENT_PROJECT_NAME})

if (${ESP_PLATFORM})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    set(EXTRA_COMPONENT_DIRS test lib)
    set(COMPONENT_REQUIRES ${CURRENT_PROJECT_NAME})
    include_directories($ENV{IDF_PATH}/components)
    add_definitions(-D_GLIBCXX_USE_C99) # for std::to_string (until using xtensa-gcc 8)
    project(smooth-test)
else ()
    include_directories($ENV{IDF_PATH}/components)
    add_subdirectory(lib/smooth)
    add_subdirectory(test/hello_world)
    add_subdirectory(test/logging)
    add_subdirectory(test/json_test)
    add_subdirectory(test/mqtt)
    add_subdirectory(test/main)
endif ()
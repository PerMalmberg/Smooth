# This CMakeLists.txt is meant for use when developing the smooth framework and running the tests.
# If you are attepmting to build smooth as a library, you'll want to use the one in the lib/smooth-folder instead.

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
#[[SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align -Wunused -Woverloaded-virtual")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Wsign-conversion")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op -Wuseless-cast -Wdouble-promotion -Wformat=2")
]]

# -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wnull-dereference

# Select the test project, then build target "main". The available projects are the names of the
# folders under "test", e.g. hello_world, mqtt etc.
set(CURRENT_PROJECT_NAME mqtt)
set(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}/test/${CURRENT_PROJECT_NAME})

# for std::to_string (until using xtensa-gcc 8)
add_definitions(-D_GLIBCXX_USE_C99)

#include_directories($ENV{IDF_PATH}/components)
add_subdirectory(test/hello_world)
add_subdirectory(test/logging)
add_subdirectory(test/json_test)
add_subdirectory(test/mqtt)
add_subdirectory(test/publish)
add_subdirectory(test/task_event_queue)
add_subdirectory(test/timer)

# These needs actual hardware to run
if( ESP_PLATFORM )
    add_subdirectory(test/spiflash)
    add_subdirectory(test/sdcard_test)
    add_subdirectory(test/jsonfile_test)
    add_subdirectory(test/sntp)
    add_subdirectory(test/security)
    add_subdirectory(test/wrover_kit_blinky)
    add_subdirectory(test/interrupt_queue)
endif()

include_directories(${CURRENT_PROJECT_DIR})

set(IDF_BUILD_ARTIFACTS ON)
set(IDF_PROJECT_EXECUTABLE main)
set(IDF_BUILD_ARTIFACTS_DIR ${CMAKE_CURRENT_LIST_DIR}/build)

add_subdirectory(test/main)

if( ESP_PLATFORM )
    # Must include this file so that targets for menuconfig etc are included.
    include($ENV{IDF_PATH}/tools/cmake/idf_functions.cmake)
endif()

add_subdirectory(lib/smooth)

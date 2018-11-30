# This CMakeLists.txt is meant for use when developing the smooth framework and running the tests.
# If you are attepmting to build smooth as a library, you'll want to use the one in the lib/smooth-folder instead.

cmake_minimum_required(VERSION 3.10)

#project(smooth_test)

#file(WRITE null.cpp "")
#add_executable(${PROJECT_NAME} null.cpp)

set(CMAKE_CXX_STANDARD 14)
#[[SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -Wshadow -Wnon-virtual-dtor")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align -Wunused -Woverloaded-virtual")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Wsign-conversion")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op -Wuseless-cast -Wdouble-promotion -Wformat=2")
]]

# -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wnull-dereference

# Select the test project, then build target "main". The available projects are the names of the
# folders under "test", e.g. hello_world, mqtt etc.
set(CURRENT_PROJECT_NAME hello_world)
set(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}/test/${CURRENT_PROJECT_NAME})

# for std::to_string (until using xtensa-gcc 8)
add_definitions(-D_GLIBCXX_USE_C99)

#include_directories($ENV{IDF_PATH}/components)
add_subdirectory(test/hello_world)
#[[add_subdirectory(test/logging)
add_subdirectory(test/json_test)
add_subdirectory(test/mqtt)
add_subdirectory(test/publish)
add_subdirectory(test/queue)
add_subdirectory(test/timer)
add_subdirectory(test/wrover_kit_blinky)
add_subdirectory(test/spiflash)
add_subdirectory(test/sdcard_test)
add_subdirectory(test/sntp)
#add_subdirectory(test/interrupt_queue) // Only compiles for ESP
]]

include_directories(${CURRENT_PROJECT_DIR})
add_subdirectory(test/main)

#target_link_libraries(${PROJECT_NAME} main ${CURRENT_PROJECT_NAME})
include_directories(${CMAKE_CURRENT_LIST_DIR})

# Make sure to add smooth last since it (IDF actually) messes with the project() function.
add_subdirectory(lib/smooth)

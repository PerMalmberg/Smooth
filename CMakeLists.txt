# This CMakeLists.txt is meant for use when developing the smooth framework and running the tests.
# If you are attepmting to build smooth as a library, you'll want to use the one in the lib/smooth-folder instead.

cmake_minimum_required(VERSION 3.10)
project(smooth_test)
set(CMAKE_CXX_STANDARD 17)

include(lib/smooth/compiler_options.cmake)
include(lib/smooth/idf_utils.cmake)

add_subdirectory(lib/smooth)
add_subdirectory(test)


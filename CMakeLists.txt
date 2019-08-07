# This CMakeLists.txt is meant for use when developing the smooth framework and running the tests.
# If you are attepmting to build smooth as a library, you'll want to use the one in the lib/smooth-folder instead.

cmake_minimum_required(VERSION 3.10)
project(smooth_test)
set(CMAKE_CXX_STANDARD 17)

# These settings controls the use of Adress Sanitizer.
# Please note that an optimization level of 0 does have a significant negative impact
# on the performance of the application when ASAN is enabled.
set(SMOOTH_ENABLE_ASAN 0)
set(SMOOTH_ASAN_OPTIMIZATION_LEVEL 1)


include(lib/smooth/compiler_options.cmake)
include(lib/smooth/idf_utils.cmake)

add_subdirectory(lib/smooth)
add_subdirectory(test)


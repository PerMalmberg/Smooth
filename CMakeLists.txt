# This CMakeLists.txt is meant for use when developing the smooth framework and running the tests.
# If you are attepmting to build smooth as a library, you'll want to use the one in the lib/smooth-folder instead.

cmake_minimum_required(VERSION 3.10)
project(smooth_test)
set(CMAKE_CXX_STANDARD 17)

include(lib/smooth/compiler_options.cmake)
include(lib/smooth/idf_utils.cmake)
include($ENV{IDF_PATH}/tools/cmake/idf.cmake)

# Select the test project, then build target "main". The available projects are the names of the
# folders under "test", e.g. hello_world, mqtt etc.
set(SELETED_TEST_PROJECT http_server_test)

set(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}/test/${SELETED_TEST_PROJECT})
set(TEST_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/test)

add_subdirectory(test/hello_world)
add_subdirectory(test/logging)
add_subdirectory(test/json_test)
add_subdirectory(test/mqtt)
add_subdirectory(test/publish)
add_subdirectory(test/task_event_queue)
add_subdirectory(test/timer)
add_subdirectory(test/secure_socket_test)
add_subdirectory(test/server_socket_test)
add_subdirectory(test/secure_server_socket_test)
add_subdirectory(test/http_server_test)

# These needs actual hardware to run
if( ESP_PLATFORM )
    add_subdirectory(test/spiflash)
    add_subdirectory(test/sdcard_test)
    add_subdirectory(test/jsonfile_test)
    add_subdirectory(test/sntp)
    add_subdirectory(test/security)
    add_subdirectory(test/wrover_kit_blinky)
    add_subdirectory(test/interrupt_queue)
else()
    add_subdirectory(test/unit_tests)
endif()

include_directories(${CURRENT_PROJECT_DIR})
add_subdirectory(lib/smooth)

add_subdirectory(test/main)
set_compile_options(${SELETED_TEST_PROJECT})


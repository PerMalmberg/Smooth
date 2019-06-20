cmake_minimum_required(VERSION 3.10)
project(smooth_test)
set(CMAKE_CXX_STANDARD 17)

set(SMOOTH_TEST_ROOT ${CMAKE_CURRENT_LIST_DIR})

include($ENV{IDF_PATH}/tools/cmake/idf.cmake)

# Select the test project, then build target "main". The available projects are the names of the
# folders under "test", e.g. hello_world, mqtt etc.
set(SELETED_TEST_PROJECT http_server_test)

set(CURRENT_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}/test/${SELETED_TEST_PROJECT})
include_directories(${SMOOTH_TEST_ROOT})

#add_subdirectory(hello_world)
#add_subdirectory(logging)
#add_subdirectory(json_test)
#add_subdirectory(mqtt)
#add_subdirectory(publish)
#add_subdirectory(task_event_queue)
#add_subdirectory(timer)
#add_subdirectory(secure_socket_test)
#add_subdirectory(server_socket_test)
#add_subdirectory(secure_server_socket_test)
add_subdirectory(http_server_test)
#
## These needs actual hardware to run
#if(${ESP_PLATFORM})
#    add_subdirectory(esp/spiflash)
#    add_subdirectory(esp/sdcard_test)
#    add_subdirectory(esp/jsonfile_test)
#    add_subdirectory(esp/sntp)
#    add_subdirectory(esp/security)
#    add_subdirectory(esp/wrover_kit_blinky)
#    add_subdirectory(esp/interrupt_queue)
#else()
#    add_subdirectory(unit_tests)
#endif()

set_compile_options(${SELETED_TEST_PROJECT})
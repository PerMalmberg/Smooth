# This CMakeLists.txt is meant for use when developing the smooth framework and running the tests.
# If you are attepmting to build smooth as a library, you'll want to use the one in the lib/smooth-folder instead.

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

# Select the test project to build for ESP
set(selected_test_project destructing_event_queues)

list(APPEND available_tests
        hello_world
        asan_test
        logging
        json_test
        mqtt
        publish
        task_event_queue
        timer
        secure_socket_test
        server_socket_test
        secure_server_socket_test
        http_server_test
        destructing_event_queues
        destructing_subscribing_event_queues)

# These tests needs actual hardware to run
if(${ESP_PLATFORM})
    list(APPEND available_tests esp/spiflash
            esp/sdcard_test
            esp/jsonfile_test
            esp/sntp
            esp/security
            esp/wrover_kit_blinky
            esp/interrupt_queue)
else()
    list(APPEND available_tests unit_tests)
endif()

list(FIND available_tests ${selected_test_project} test_found)
if(${test_found} EQUAL -1)
    message(FATAL_ERROR "'${selected_test_project}' isn't a valid test project")
endif()

set(EXTRA_COMPONENT_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/smooth_component
        ${CMAKE_CURRENT_SOURCE_DIR}/test/${selected_test_project})

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(smooth_test)


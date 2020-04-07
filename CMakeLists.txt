#[[
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
]]

# This CMakeLists.txt is meant for use when developing the smooth framework and running the tests.

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

# Select the test project to build
set(selected_test_project starter_example)

# For Linux builds, you may enable address sanitizer
set(SMOOTH_ENABLE_ASAN 0)
set(SMOOTH_ASAN_OPTIMIZATION_LEVEL 1)

list(APPEND available_tests
        starter_example
        access_point
        logging
        mqtt
        publish
        task_event_queue
        timer
        secure_socket_test
        server_socket_test
        secure_server_socket_test
        http_server_test
        destructing_event_queues
        destructing_subscribing_event_queues
        security
        sntp
        hw_interrupt_queue
        hw_sdcard_test
        hw_spiflash
        hw_jsonfile_test
        linux_asan_test
        linux_unit_tests
        hw_wrover_kit_blinky
        i2c_bme280_test
        spi_4_line_devices_test
        i2c_dht12_test
        i2c_rtc8563_test
        )

list(FIND available_tests ${selected_test_project} test_found)
if(${test_found} EQUAL -1)
    message(FATAL_ERROR "'${selected_test_project}' not found or not a valid test project for the current platform.")
endif()

if(${ESP_PLATFORM})
    message(STATUS "Compiling for ESP")
    set(EXTRA_COMPONENT_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/smooth_component
            ${CMAKE_CURRENT_SOURCE_DIR}/test/${selected_test_project})

    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(${selected_test_project})
else()
    message(STATUS "Compiling for Host Platform")
    if(${APPLE})
        include_directories(SYSTEM /usr/local/include)
        link_directories(/usr/local/lib)
    endif()
    add_subdirectory(mock-idf)
    add_subdirectory(lib)
    add_subdirectory(test/${selected_test_project})
endif()

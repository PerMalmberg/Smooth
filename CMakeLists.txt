# Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
# Copyright (C) 2019 Per Malmberg (https://github.com/PerMalmberg)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# This CMakeLists.txt is meant for use when developing the smooth framework and running the tests.

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

# Select the test project to build
set(selected_test_project http_server_test)

# For Linux builds, you may enable address sanitizer
set(SMOOTH_ENABLE_ASAN 1)
set(SMOOTH_ASAN_OPTIMIZATION_LEVEL 1)

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
    list(APPEND available_tests
            hw_spiflash
            hw_sdcard_test
            hw_jsonfile_test
            hw_sntp
            hw_security
            hw_wrover_kit_blinky
            hw_interrupt_queue)
endif()

list(FIND available_tests ${selected_test_project} test_found)
if(${test_found} EQUAL -1)
    message(FATAL_ERROR "'${selected_test_project}' isn't a valid test project")
endif()

if(${ESP_PLATFORM})
    message(STATUS "Compiling for ESP")
    set(EXTRA_COMPONENT_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/smooth_component
            ${CMAKE_CURRENT_SOURCE_DIR}/test/${selected_test_project})

    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(${selected_test_project})
else()
    message(STATUS "Compiling for Linux")
    add_subdirectory(lib)
    add_subdirectory(test/${selected_test_project})
    add_subdirectory(test/unit_tests)
endif()

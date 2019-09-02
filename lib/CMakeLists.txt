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

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(smooth)
include(files.cmake)
include(compiler_options.cmake)

add_definitions(-DCONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE=512)
add_definitions(-DCONFIG_SMOOTH_MAX_MQTT_OUTGOING_MESSAGES=10)
add_definitions(-DSMOOTH_MQTT_LOGGING_LEVEL=1)

add_library(${PROJECT_NAME} ${SMOOTH_SOURCES})
target_link_libraries(${PROJECT_NAME} mbedtls mbedx509 mbedcrypto sodium mock-idf)
set_compile_options(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${SMOOTH_LIB_ROOT}/smooth/include
            $ENV{IDF_PATH}/components/json/cJSON
            ${CMAKE_CURRENT_LIST_DIR}/../externals/nlohman_json/include
        )

# When building for Linux, mock-idf include paths must be make available.
file(GLOB mock_components LIST_DIRECTORIES true
        ${CMAKE_CURRENT_LIST_DIR}/../mock-idf/components/* )

foreach(mock ${mock_components})
    target_include_directories(${PROJECT_NAME} PUBLIC ${mock}/include)
endforeach()

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

project(mock-idf C CXX)
set(SOURCES components/esp_wifi/esp_wifi.cpp
        components/esp_common/esp_err.cpp)

add_library(${PROJECT_NAME} ${SOURCES})

file(GLOB mock_components LIST_DIRECTORIES true
        ${CMAKE_CURRENT_LIST_DIR}/components/*)

foreach(mock_comp ${mock_components})
    target_include_directories(${PROJECT_NAME} PUBLIC ${mock_comp}/include)
endforeach()


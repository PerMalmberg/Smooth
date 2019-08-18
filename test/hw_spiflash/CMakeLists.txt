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

get_filename_component(TEST_PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/generated_test_smooth_${TEST_PROJECT}.cpp)
configure_file(${CMAKE_CURRENT_LIST_DIR}/../test.cpp.in ${TEST_SRC})
set(TEST_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

# As project() isn't scriptable and the entire file is evaluated we work around the limitation by generating
# the actual file used for the respective platform.
if(NOT "${COMPONENT_DIR}" STREQUAL "")
    configure_file(${CMAKE_CURRENT_LIST_DIR}/../test_project_template_esp.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/generated_test_esp.cmake @ONLY)
    include(${CMAKE_CURRENT_BINARY_DIR}/generated_test_esp.cmake)
else()
    cmessage(FATAL_ERROR "The ${COMPONENT_DIR} project can only be compiled and run for the ESP.")
endif()

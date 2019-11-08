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
    configure_file(${CMAKE_CURRENT_LIST_DIR}/../test_project_template_linux.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/generated_test_linux.cmake @ONLY)
    include(${CMAKE_CURRENT_BINARY_DIR}/generated_test_linux.cmake)
endif()


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



cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(smooth)
include(files.cmake)
include(compiler_options.cmake)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../externals/fmt ${CMAKE_BINARY_DIR}/externals/fmt)

add_library(${PROJECT_NAME} ${SMOOTH_SOURCES})
target_link_libraries(${PROJECT_NAME} mbedtls mbedx509 mbedcrypto sodium mock-idf fmt)
set_compile_options(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC
            ${SMOOTH_LIB_ROOT}/smooth/include
            ${CMAKE_CURRENT_LIST_DIR}/../externals/nlohmann_json/include
        )

# When building for Linux, mock-idf include paths must be make available.
file(GLOB mock_components LIST_DIRECTORIES true
        ${CMAKE_CURRENT_LIST_DIR}/../mock-idf/components/* )

foreach(mock ${mock_components})
    target_include_directories(${PROJECT_NAME} PUBLIC ${mock}/include)
endforeach()

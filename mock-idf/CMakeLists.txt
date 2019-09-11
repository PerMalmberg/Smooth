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

project(mock-idf C CXX)

file(GLOB_RECURSE mock_sources ${CMAKE_CURRENT_LIST_DIR}/components/*.cpp)

set(SOURCES ${mock_sources})

add_library(${PROJECT_NAME} ${SOURCES})

file(GLOB mock_components LIST_DIRECTORIES true
        ${CMAKE_CURRENT_LIST_DIR}/components/*)

foreach(mock_comp ${mock_components})
    target_include_directories(${PROJECT_NAME} PUBLIC ${mock_comp}/include)
endforeach()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/components/lwip/include/lwip/apps)

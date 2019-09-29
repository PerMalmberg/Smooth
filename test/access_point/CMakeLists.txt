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

cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

set(smooth_root ${CMAKE_CURRENT_LIST_DIR}/../..)

get_filename_component(project_name ${CMAKE_CURRENT_LIST_DIR} NAME)

if(${ESP_PLATFORM})
    set(EXTRA_COMPONENT_DIRS
            ${smooth_root}/smooth_component)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(${project_name})
else()
    project("${project_name}.elf")

    if(${APPLE})
        target_link_directories(${PROJECT_NAME} /usr/local/lib)
        target_include_directories(${PROJECT_NAME} SYSTEM /usr/local/include)
    endif()

    file(GLOB project_sources main/*)
    add_executable(${PROJECT_NAME} ${project_sources})
    target_link_libraries(${PROJECT_NAME} smooth.elf pthread)
endif()

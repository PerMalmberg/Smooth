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

# Full path needed when including this component from another project
set(CMAKE_CXX_STANDARD 17)

include(${COMPONENT_DIR}/../lib/compiler_options.cmake)
include(${COMPONENT_DIR}/../lib/files.cmake)

file(TO_CMAKE_PATH "$ENV{IDF_PATH}" normalized_path)

idf_component_register(SRCS ${SMOOTH_SOURCES}
                        INCLUDE_DIRS
                            ${SMOOTH_LIB_ROOT}/smooth/include
                            ${normalized_path}/components
                            ${CMAKE_CURRENT_LIST_DIR}/../externals/nlohmann_json/include
                            ${CMAKE_CURRENT_LIST_DIR}/../externals/fmt/include
                        REQUIRES
                            mbedtls
                            json
                            pthread
                            nvs_flash
                            fatfs
                            libsodium
                            )

add_subdirectory(${COMPONENT_DIR}/../externals/fmt ${CMAKE_BINARY_DIR}/externals/fmt)

target_link_libraries(${COMPONENT_LIB} fmt)

set_compile_options(${COMPONENT_LIB})

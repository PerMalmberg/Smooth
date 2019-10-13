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

function(set_compile_options target)
    target_compile_options(${target} PUBLIC $<$<COMPILE_LANGUAGE:CXX>: -fno-rtti>)
    target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>: -Wnon-virtual-dtor -Wold-style-cast>)
    target_compile_options(${target} PRIVATE
                                            -Werror
                                            -Wall
                                            -Wextra
                                            -Wcast-align
                                            -Wunused
                                            -Wconversion
                                            -Wsign-conversion
                                            -Wlogical-op
                                            -Wdouble-promotion
                                            -Wformat=2
                                            -Wimplicit-fallthrough
                                            -Wcast-align
                                            -Wmisleading-indentation
                                            -Wduplicated-cond
                                            -Wduplicated-branches
                                            -Wnull-dereference)

    if( NOT "${ESP_PLATFORM}" )
        if(${SMOOTH_ENABLE_ASAN})
            if(NOT DEFINED SMOOTH_ASAN_OPTIMIZATION_LEVEL)
                message(FATAL_ERROR "SMOOTH_ASAN_OPTIMIZATION_LEVEL not set")
            endif()

            message(STATUS "ASAN enabled, expect performance degradation")
            target_compile_options(${target} PRIVATE -fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope -g -O${SMOOTH_ASAN_OPTIMIZATION_LEVEL})
            target_link_libraries(${target} -fsanitize=address)
        endif()
    endif()
endfunction()

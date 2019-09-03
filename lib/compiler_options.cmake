# Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
# Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
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

function(set_compile_options target)
    target_compile_options(${target} PRIVATE -Werror -Wall -Wextra)
    target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>: -Wnon-virtual-dtor>)
    #target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>: -Wuseless-cast>)
    target_compile_options(${target} PRIVATE -Wcast-align -Wunused )
    target_compile_options(${target} PRIVATE -Wconversion -Wsign-conversion)
    target_compile_options(${target} PRIVATE -Wlogical-op -Wdouble-promotion -Wformat=2)
    target_compile_options(${target} PUBLIC $<$<COMPILE_LANGUAGE:CXX>: -fno-rtti>)

    if( NOT "${ESP_PLATFORM}" )
        target_compile_options(${target} PRIVATE -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wnull-dereference)

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

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

# Full path needed when including this component from another project
set(CMAKE_CXX_STANDARD 17)

include(${COMPONENT_DIR}/../lib/compiler_options.cmake)
include(${COMPONENT_DIR}/../lib/files.cmake)

file(TO_CMAKE_PATH "$ENV{IDF_PATH}" normalized_path)

idf_component_register(
	SRCS
		${SMOOTH_SOURCES}
    INCLUDE_DIRS
		${smooth_dir}/include
        ${normalized_path}/components
        ${CMAKE_CURRENT_LIST_DIR}/../externals/nlohman_json/include
		${CMAKE_CURRENT_LIST_DIR}/../externals/hinnant_date/include
    REQUIRES
        mbedtls
        json
        pthread
        nvs_flash
        fatfs
        libsodium
)

set_compile_options(${COMPONENT_LIB})
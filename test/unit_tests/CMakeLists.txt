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

project(unit_test)

add_executable(${PROJECT_NAME}
        url_encoding_test.cpp
        PathTest.cpp
        FSLockTest.cpp
        MIMEParserTest.cpp
        StringUtilTest.cpp
        TemplateProcessorTest.cpp
        HashTest.cpp
        MountPointTest.cpp)

target_include_directories(${PROJECT_NAME}
        PRIVATE ${SMOOTH_TEST_ROOT})

target_link_libraries(${PROJECT_NAME} smooth pthread)

include(../../lib/compiler_options.cmake)
set_compile_options(${PROJECT_NAME})

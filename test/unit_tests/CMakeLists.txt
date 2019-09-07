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



project(unit_test)

add_executable(${PROJECT_NAME}
        url_encoding_test.cpp
        PathTest.cpp
        FSLockTest.cpp
        MIMEParserTest.cpp
        StringUtilTest.cpp
        TemplateProcessorTest.cpp
        HashTest.cpp
        FlashMountTest.cpp
        JsonTest.cpp)

target_include_directories(${PROJECT_NAME}
        PRIVATE ${SMOOTH_TEST_ROOT})

target_link_libraries(${PROJECT_NAME} smooth pthread)

include(../../lib/compiler_options.cmake)
set_compile_options(${PROJECT_NAME})

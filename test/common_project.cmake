get_filename_component(TEST_PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)

message(STATUS "Building: ${TEST_PROJECT}")

file(GLOB SOURCES *.h *.cpp)

set(TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/generated_test_smooth_${TEST_PROJECT}.cpp)

configure_file(${SMOOTH_TEST_ROOT}/test.cpp.in ${TEST_SRC})
list(APPEND SOURCES ${TEST_SRC})

project(${TEST_PROJECT})
add_executable(${TEST_PROJECT} ${SOURCES})

if(${ESP_PLATFORM})
    idf_build_executable(${TEST_PROJECT})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    __project_info("")
else()
    if(${SMOOTH_ENABLE_ASAN})
        target_link_libraries(${PROJECT_NAME} asan)
        message(STATUS "ASAN is enabled for project ${PROJECT_NAME}")
    endif()
endif()

target_include_directories(${TEST_PROJECT} PRIVATE $ENV{IDF_PATH}/components ${CMAKE_CURRENT_SOURCE_DIR} ${SMOOTH_TEST_ROOT})
target_link_libraries(${TEST_PROJECT} smooth)
set_compile_options(${TEST_PROJECT})

# Place binary output in top-level build directory so that idf.py can find it.
set_target_properties( ${TEST_PROJECT}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")


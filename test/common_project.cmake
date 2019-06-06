get_filename_component(TEST_PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)

message(STATUS "Building: ${TEST_PROJECT}")

file(GLOB SOURCES *.h *.cpp)

project(${TEST_PROJECT})
add_library(${TEST_PROJECT} ${SOURCES})
target_link_libraries(${TEST_PROJECT} smooth)
target_include_directories(${TEST_PROJECT}
        PRIVATE $ENV{IDF_PATH}/components
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

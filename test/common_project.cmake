get_filename_component(SELETED_TEST_PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)

message(STATUS "Building: ${PROJECT}")

file(GLOB SOURCES *.h *.cpp)

project(${SELETED_TEST_PROJECT})
add_library(${SELETED_TEST_PROJECT} ${SOURCES})
target_link_libraries(${SELETED_TEST_PROJECT} smooth)
target_include_directories(${SELETED_TEST_PROJECT}
        PRIVATE $ENV{IDF_PATH}/components
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

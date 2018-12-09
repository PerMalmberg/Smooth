get_filename_component(CURRENT_PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

message(STATUS "Building: ${PROJECT}")

file(GLOB SOURCES *.h *.cpp)

project(${CURRENT_PROJECT_NAME})
add_library(${CURRENT_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CURRENT_PROJECT_NAME} smooth)
target_include_directories(${CURRENT_PROJECT_NAME}
        PRIVATE $ENV{IDF_PATH}/components
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

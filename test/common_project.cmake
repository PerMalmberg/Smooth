get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)

message(STATUS "Building: ${PROJECT}")

file(GLOB SOURCES *.h *.cpp)

if (${ESP_PLATFORM})
    set(COMPONENT_SRCS ${SOURCES})
    set(COMPONENT_REQUIRES smooth json)
    set(COMPONENT_ADD_INCLUDEDIRS ${COMPONENT_PATH})
    register_component()
else ()
    project(${PROJECT})
    add_library(${PROJECT} ${SOURCES})
    target_link_libraries(${PROJECT} smooth)
    target_include_directories(${PROJECT}
            PRIVATE $ENV{IDF_PATH}/components
            PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif ()
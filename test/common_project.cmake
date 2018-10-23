get_filename_component(PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)

message(STATUS "Building: ${PROJECT}")

set(SOURCES
        ${PROJECT}.cpp
        ${PROJECT}.h)

if (${ESP_PLATFORM})
    set(COMPONENT_SRCS ${SOURCES})
    set(COMPONENT_REQUIRES smooth)
    set(COMPONENT_ADD_INCLUDEDIRS ${COMPONENT_PATH})
    register_component()
else ()
    project(${PROJECT})
    add_library(${PROJECT} ${SOURCES})
    target_link_libraries(${PROJECT} smooth)
    target_include_directories(${PROJECT} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif ()
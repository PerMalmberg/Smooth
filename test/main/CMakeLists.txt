
configure_file(${CMAKE_CURRENT_LIST_DIR}/test.cpp.in ${CMAKE_CURRENT_LIST_DIR}/generated_test.cpp)

if (${ESP_PLATFORM})
    set(COMPONENT_SRCS generated_test.cpp)
    register_component()
else ()
    project(main)
    add_executable(${PROJECT_NAME} generated_test.cpp)
    target_link_libraries(${PROJECT_NAME} ${CURRENT_PROJECT_NAME})
endif ()
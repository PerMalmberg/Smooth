project(main)

configure_file(${CMAKE_CURRENT_LIST_DIR}/test.cpp.in ${CMAKE_CURRENT_LIST_DIR}/generated_test.cpp)
set(SOURCES generated_test.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} smooth ${CURRENT_PROJECT_NAME})

# Place binary output in top-level build directory so that idf.py can find it.
set_target_properties( ${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../.."
        )


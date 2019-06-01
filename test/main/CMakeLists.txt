configure_file(${CMAKE_CURRENT_LIST_DIR}/test.cpp.in ${CMAKE_CURRENT_LIST_DIR}/generated_test.cpp)
set(SOURCES generated_test.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} smooth ${SELETED_TEST_PROJECT})

# Place binary output in top-level build directory so that idf.py can find it.
set_target_properties( ${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../.."
        )

if(ESP_PLATFORM)
    idf_build_executable(${PROJECT_NAME})
else()
    target_link_libraries(${PROJECT_NAME} asan)
endif()

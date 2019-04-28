project(url_encoding_test)

add_executable(${PROJECT_NAME} url_encoding_test.cpp)

target_include_directories(${PROJECT_NAME}
        PRIVATE ${TEST_ROOT_DIR})
target_link_libraries(${PROJECT_NAME} smooth asan)


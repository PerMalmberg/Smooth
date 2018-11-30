project(main)

configure_file(${CMAKE_CURRENT_LIST_DIR}/test.cpp.in ${CMAKE_CURRENT_LIST_DIR}/generated_test.cpp)
set(SOURCES generated_test.cpp)
add_executable(${PROJECT_NAME} generated_test.cpp)
target_link_libraries(${PROJECT_NAME} smooth ${CURRENT_PROJECT_NAME})


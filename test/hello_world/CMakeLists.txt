get_filename_component(TEST_PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set(TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/generated_test_smooth_${TEST_PROJECT}.cpp)
configure_file(${CMAKE_CURRENT_LIST_DIR}/../test.cpp.in ${TEST_SRC})
set(TEST_PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})

# As project() isn't scriptable and the entire file is evaluated we work around the limitation by generating
# the actual file used for the respective platform.
if(NOT "${COMPONENT_DIR}" STREQUAL "")
    configure_file(${CMAKE_CURRENT_LIST_DIR}/../test_project_template_esp.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/generated_test_esp.cmake @ONLY)
    include(${CMAKE_CURRENT_BINARY_DIR}/generated_test_esp.cmake)
else()
    configure_file(${CMAKE_CURRENT_LIST_DIR}/../test_project_template_linux.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/generated_test_linux.cmake @ONLY)
    include(${CMAKE_CURRENT_BINARY_DIR}/generated_test_linux.cmake)
endif()


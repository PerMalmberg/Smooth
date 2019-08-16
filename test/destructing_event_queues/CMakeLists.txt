get_filename_component(TEST_PROJECT ${CMAKE_CURRENT_SOURCE_DIR} NAME)

message(STATUS "Building: ${TEST_PROJECT}")

file(GLOB SOURCES *.h *.cpp)

set(TEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/generated_test_smooth_${TEST_PROJECT}.cpp)
configure_file(${CMAKE_CURRENT_LIST_DIR}/../test.cpp.in ${TEST_SRC})
list(APPEND SOURCES ${TEST_SRC})

idf_component_register(SRCS ${SOURCES}
        INCLUDE_DIRS "."
        PRIV_REQUIRES smooth_component
        )


project(unit_test)

add_executable(${PROJECT_NAME}
        url_encoding_test.cpp
        PathTest.cpp
        FSLockTest.cpp
        MIMEParserTest.cpp
        StringUtilTest.cpp
        TemplateProcessorTest.cpp
        HashTest.cpp)

target_include_directories(${PROJECT_NAME}
        PRIVATE ${SMOOTH_TEST_ROOT})
target_link_libraries(${PROJECT_NAME} smooth )


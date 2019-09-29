file(GLOB src *)

if(${ESP_PLATFORM})
    idf_component_register(SRCS ${src} REQUIRES smooth_component)
else()
    project(example.elf)

    add_executable(${PROJECT_NAME} ${src})
    target_link_libraries(${PROJECT_NAME} smooth.elf pthread mbedtls mbedx509 mbedcrypto sodium mock-idf)

    target_include_directories(${PROJECT_NAME}
            PUBLIC
            ${SMOOTH_LIB_ROOT}/smooth/include
            ${CMAKE_CURRENT_LIST_DIR}/../externals/nlohmann_json/include
            )

    # When building for Linux, mock-idf include paths must be make available.
    file(GLOB mock_components LIST_DIRECTORIES true
            ${CMAKE_CURRENT_LIST_DIR}/../mock-idf/components/* )

    foreach(mock ${mock_components})
        target_include_directories(${PROJECT_NAME} PUBLIC ${mock}/include)
    endforeach()
endif()
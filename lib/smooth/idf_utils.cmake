function(smooth_verify_idf_path)
    if (NOT EXISTS "$ENV{IDF_PATH}/Kconfig")
        message(FATAL_ERROR "Environment IDF_PATH must be set to an existing IDF-installation. IDF_PATH: '$ENV{IDF_PATH}'")
    else ()
        message(STATUS "IDF_PATH appears valid.")
    endif ()
endfunction()

function(smooth_configure_link target)
    if(${CMAKE_CXX_COMPILER} MATCHES "xtensa")
        message(STATUS "Smooth: Importing IDF components.")

        include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
        target_compile_definitions(smooth PUBLIC ESP_PLATFORM)

        set(comps fatfs sdmmc spi_flash nvs_flash wear_levelling libsodium lwip json)

        idf_build_component("${CMAKE_CURRENT_LIST_DIR}/smooth_idf_component")

        idf_build_process(esp32
                COMPONENTS esp32 freertos esptool_py ${comps}
                SDKCONFIG ${CMAKE_CURRENT_LIST_DIR}/sdkconfig
                BUILD_DIR ${CMAKE_BINARY_DIR})


        foreach(c ${comps})
            target_link_libraries(${target} idf::${c})
        endforeach()
    else()
        message(STATUS "Smooth: Building for native Linux.")
        target_include_directories(${target} PRIVATE $ENV{IDF_PATH}/components/json/cJSON)

        target_link_libraries(${target} pthread mbedtls mbedx509 mbedcrypto)

        message(STATUS "Compiling for native Linux: Using default values for Smooth-MQTT")
        add_definitions("-DCONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE=3500")
        add_definitions("-DCONFIG_SMOOTH_MAX_MQTT_OUTGOING_MESSAGES=10")
        add_definitions("-DCONFIG_SMOOTH_MQTT_LOGGING_LEVEL=0")
    endif()
endfunction()
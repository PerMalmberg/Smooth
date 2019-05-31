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
            target_compile_definitions(${target} PRIVATE ESP_PLATFORM)

        include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
        set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/smooth_idf_component")

        idf_build_process(esp32
                # try and trim the build; additional components
                # will be included as needed based on dependency tree
                #
                # although esptool_py does not generate static library,
                # processing the component is needed for flashing related
                # targets and file generation
                COMPONENTS esp32 freertos esptool_py
                SDKCONFIG ${CMAKE_CURRENT_LIST_DIR}/sdkconfig
                BUILD_DIR ${CMAKE_BINARY_DIR})

        target_link_libraries(${target} idf::esp32)
    else()
        message(STATUS "Smooth: Building for native Linux.")

        message(STATUS "Compiling for native Linux: Using default values for Smooth-MQTT")
        add_definitions("-DCONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE=3500")
        add_definitions("-DCONFIG_SMOOTH_MAX_MQTT_OUTGOING_MESSAGES=10")
        add_definitions("-DCONFIG_SMOOTH_MQTT_LOGGING_LEVEL=0")
    endif()
endfunction()
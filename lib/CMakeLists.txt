cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(smooth)
include(files.cmake)
include(compiler_options.cmake)

add_definitions(-DCONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE=512)
add_definitions(-DCONFIG_SMOOTH_MAX_MQTT_OUTGOING_MESSAGES=10)
add_definitions(-DSMOOTH_MQTT_LOGGING_LEVEL=1)


add_library(${PROJECT_NAME} ${SMOOTH_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC
        ${SMOOTH_LIB_ROOT}/smooth/include
        $ENV{IDF_PATH}/components/json/cJSON)

set_compile_options(${PROJECT_NAME})

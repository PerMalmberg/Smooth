project(smooth)

include(${CMAKE_CURRENT_LIST_DIR}/idf_utils.cmake)

smooth_verify_idf_path()

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        application/network/http/HTTPPacket.cpp
        application/network/http/URLEncoding.cpp
        application/network/http/http_utils.cpp
        application/network/http/responses/EmptyResponse.cpp
        application/network/http/responses/FileContentResponse.cpp
        application/network/http/responses/Response.cpp
        application/network/mqtt/packet/ConnAck.cpp
        application/network/mqtt/packet/Connect.cpp
        application/network/mqtt/packet/Disconnect.cpp
        application/network/mqtt/packet/MQTTPacket.cpp
        application/network/mqtt/packet/MQTTProtocol.cpp
        application/network/mqtt/packet/MQTTProtocolDefinitions.cpp
        application/network/mqtt/packet/PacketDecoder.cpp
        application/network/mqtt/packet/PacketIdentifierFactory.cpp
        application/network/mqtt/packet/PingReq.cpp
        application/network/mqtt/packet/PingResp.cpp
        application/network/mqtt/packet/PubAck.cpp
        application/network/mqtt/packet/PubComp.cpp
        application/network/mqtt/packet/Publish.cpp
        application/network/mqtt/packet/PubRec.cpp
        application/network/mqtt/packet/PubRel.cpp
        application/network/mqtt/packet/SubAck.cpp
        application/network/mqtt/packet/Subscribe.cpp
        application/network/mqtt/packet/UnsubAck.cpp
        application/network/mqtt/packet/Unsubscribe.cpp
        application/network/mqtt/state/ConnectedState.cpp
        application/network/mqtt/state/ConnectToBrokerState.cpp
        application/network/mqtt/state/DisconnectedState.cpp
        application/network/mqtt/state/DisconnectState.cpp
        application/network/mqtt/state/MQTTBaseState.cpp
        application/network/mqtt/state/RunState.cpp
        application/network/mqtt/MqttClient.cpp
        application/network/mqtt/Publication.cpp
        application/network/mqtt/Subscription.cpp
        core/ipc/QueueNotification.cpp
        core/filesystem/File.cpp
        core/filesystem/FSLock.cpp
        core/filesystem/Path.cpp
        core/network/IPv4.cpp
        core/network/IPv6.cpp
        core/network/MbedTLSContext.cpp
        core/network/SocketDispatcher.cpp
        core/network/CommonSocket.cpp
        core/timer/ElapsedTime.cpp
        core/timer/Timer.cpp
        core/timer/TimerService.cpp
        core/Application.cpp
        core/json/Value.cpp
        core/json/JsonFile.cpp
        core/Task.cpp
        include/smooth/application/display/ST7735.h
        include/smooth/application/io/i2c/ADS1115.h
        include/smooth/application/io/i2c/MCP23017.h
        include/smooth/application/network/http/HTTPServerClient.h
        include/smooth/application/network/http/HTTPServer.h
        include/smooth/application/network/http/HTTPProtocol.h
        include/smooth/application/network/http/URLEncoding.h
        include/smooth/application/network/http/http_utils.h
        include/smooth/application/network/http/responses/EmptyResponse.h
        include/smooth/application/network/http/responses/FileContentResponse.h
        include/smooth/application/network/http/responses/IRequestResponeOperation.h
        include/smooth/application/network/http/responses/Response.h
        include/smooth/application/network/mqtt/event/BaseEvent.h
        include/smooth/application/network/mqtt/event/ConnectEvent.h
        include/smooth/application/network/mqtt/event/DisconnectEvent.h
        include/smooth/application/network/mqtt/packet/ConnAck.h
        include/smooth/application/network/mqtt/packet/Connect.h
        include/smooth/application/network/mqtt/packet/Disconnect.h
        include/smooth/application/network/mqtt/packet/IPacketReceiver.h
        include/smooth/application/network/mqtt/packet/MQTTProtocol.h
        include/smooth/application/network/mqtt/packet/PacketDecoder.h
        include/smooth/application/network/mqtt/packet/PacketIdentifierFactory.h
        include/smooth/application/network/mqtt/packet/PingReq.h
        include/smooth/application/network/mqtt/packet/PingResp.h
        include/smooth/application/network/mqtt/packet/PubAck.h
        include/smooth/application/network/mqtt/packet/PubComp.h
        include/smooth/application/network/mqtt/packet/Publish.h
        include/smooth/application/network/mqtt/packet/PubRec.h
        include/smooth/application/network/mqtt/packet/PubRel.h
        include/smooth/application/network/mqtt/packet/SubAck.h
        include/smooth/application/network/mqtt/packet/Subscribe.h
        include/smooth/application/network/mqtt/packet/UnsubAck.h
        include/smooth/application/network/mqtt/packet/Unsubscribe.h
        include/smooth/application/network/mqtt/packet/MQTTPacket.h
        include/smooth/application/network/mqtt/state/ConnectedState.h
        include/smooth/application/network/mqtt/state/ConnectToBrokerState.h
        include/smooth/application/network/mqtt/state/DisconnectedState.h
        include/smooth/application/network/mqtt/state/DisconnectState.h
        include/smooth/application/network/mqtt/state/IdleState.h
        include/smooth/application/network/mqtt/state/MQTTBaseState.h
        include/smooth/application/network/mqtt/state/MqttFSM.h
        include/smooth/application/network/mqtt/state/MqttFsmConstants.h
        include/smooth/application/network/mqtt/state/RunState.h
        include/smooth/application/network/mqtt/state/StartupState.h
        include/smooth/application/network/mqtt/IMqttClient.h
        include/smooth/application/network/mqtt/InFlight.h
        include/smooth/application/network/mqtt/Logging.h
        include/smooth/application/network/mqtt/MqttClient.h
        include/smooth/application/network/mqtt/MQTTProtocolDefinitions.h
        include/smooth/application/network/mqtt/Publication.h
        include/smooth/application/network/mqtt/Subscription.h
        $ENV{IDF_PATH}/components/json/cJSON/cJSON.c
        $ENV{IDF_PATH}/components/json/cJSON/cJSON.h
        )

if (${ESP_PLATFORM})
    list(APPEND SOURCES
            include/smooth/core/network/Wifi.h
            core/logging/idf/idf_log.cpp
            core/network/Wifi.cpp
            core/io/Input.cpp
            core/io/Output.cpp
            core/io/InterruptInput.cpp
            core/io/i2c/I2CCommandLink.cpp
            core/io/i2c/I2CMasterDevice.cpp
            core/io/i2c/Master.cpp
            core/filesystem/SPIFlash.cpp
            core/filesystem/SDCard.cpp
            core/filesystem/SPISDCard.cpp
            include/smooth/core/filesystem/SPIFlash.h
            include/smooth/core/filesystem/SDCard.h
            include/smooth/core/filesystem/SPISDCard.h
            core/sntp/Sntp.cpp
            include/smooth/core/sntp/Sntp.h
            core/filesystem/MMCSDCard.cpp
            include/smooth/core/filesystem/MMCSDCard.h
            include/smooth/core/io/Input.h
            include/smooth/core/io/Output.h
            include/smooth/core/io/i2c/I2CCommandLink.h
            include/smooth/core/io/InterruptInput.h
            include/smooth/core/io/i2c/I2CMasterDevice.h
            include/smooth/core/io/i2c/Master.h
            application/io/i2c/MCP23017.cpp
            application/io/i2c/ADS1115.cpp
            application/io/i2c/MCP23017.cpp
            application/io/i2c/BME280.cpp
            application/io/wiegand/Wiegand.cpp
            application/security/PasswordHash.cpp
            include/smooth/application/security/PasswordHash.h
            )
else()
        list(APPEND SOURCES
                core/logging/posix/posix_log.cpp
        )
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES})

if (NOT ESP_PLATFORM)
    target_link_libraries(${PROJECT_NAME} pthread mbedtls mbedx509 mbedcrypto)
endif()

include(compiler_options.cmake)
set_compile_options(${PROJECT_NAME})

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE ${CMAKE_BINARY_DIR}
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
        PUBLIC $ENV{IDF_PATH}/components
)

smooth_configure_link(${PROJECT_NAME})



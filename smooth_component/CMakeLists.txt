# Smooth - C++ framework for writing applications based on Espressif's ESP-IDF.
# Copyright (C) 2017 Per Malmberg (https://github.com/PerMalmberg)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

set(SMOOTH_LIB_ROOT ${CMAKE_CURRENT_LIST_DIR}/../lib/smooth)

set(SOURCES
        ${SMOOTH_LIB_ROOT}/application/network/http/HTTPServerClient.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/regular/HTTPPacket.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/URLEncoding.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/http_utils.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/HTTPProtocol.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/regular/RegularHTTPProtocol.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/regular/MIMEParser.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/regular/HTTPHeaderDef.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/regular/responses/ErrorResponse.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/regular/responses/FileContentResponse.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/regular/responses/StringResponse.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/regular/responses/HeaderOnlyResponse.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/regular/TemplateProcessor.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/websocket/responses/WSResponse.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/websocket/WebsocketProtocol.cpp
        ${SMOOTH_LIB_ROOT}/application/network/http/websocket/WebSocketServer.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/ConnAck.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/Connect.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/Disconnect.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/MQTTPacket.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/MQTTProtocol.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/MQTTProtocolDefinitions.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/PacketDecoder.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/PacketIdentifierFactory.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/PingReq.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/PingResp.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/PubAck.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/PubComp.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/Publish.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/PubRec.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/PubRel.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/SubAck.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/Subscribe.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/UnsubAck.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/packet/Unsubscribe.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/state/ConnectedState.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/state/ConnectToBrokerState.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/state/DisconnectedState.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/state/DisconnectState.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/state/MQTTBaseState.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/state/RunState.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/MqttClient.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/Publication.cpp
        ${SMOOTH_LIB_ROOT}/application/network/mqtt/Subscription.cpp
        ${SMOOTH_LIB_ROOT}/application/hash/sha.cpp
        ${SMOOTH_LIB_ROOT}/application/hash/base64.cpp
        ${SMOOTH_LIB_ROOT}/core/ipc/QueueNotification.cpp
        ${SMOOTH_LIB_ROOT}/core/filesystem/filesystem.cpp
        ${SMOOTH_LIB_ROOT}/core/filesystem/File.cpp
        ${SMOOTH_LIB_ROOT}/core/filesystem/FSLock.cpp
        ${SMOOTH_LIB_ROOT}/core/filesystem/Path.cpp
        ${SMOOTH_LIB_ROOT}/core/network/IPv4.cpp
        ${SMOOTH_LIB_ROOT}/core/network/IPv6.cpp
        ${SMOOTH_LIB_ROOT}/core/network/MbedTLSContext.cpp
        ${SMOOTH_LIB_ROOT}/core/network/SocketDispatcher.cpp
        ${SMOOTH_LIB_ROOT}/core/network/CommonSocket.cpp
        ${SMOOTH_LIB_ROOT}/core/timer/ElapsedTime.cpp
        ${SMOOTH_LIB_ROOT}/core/timer/Timer.cpp
        ${SMOOTH_LIB_ROOT}/core/timer/TimerService.cpp
        ${SMOOTH_LIB_ROOT}/core/util/string_util.cpp
        ${SMOOTH_LIB_ROOT}/core/Application.cpp
        ${SMOOTH_LIB_ROOT}/core/json/Value.cpp
        ${SMOOTH_LIB_ROOT}/core/json/JsonFile.cpp
        ${SMOOTH_LIB_ROOT}/core/Task.cpp
        ${SMOOTH_LIB_ROOT}/core/SystemStatistics.cpp
        ${SMOOTH_LIB_ROOT}/include/smooth/application/io/i2c/ADS1115.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/io/i2c/MCP23017.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/HTTPServerClient.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/HTTPServer.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/HTTPProtocol.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/URLEncoding.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/http_utils.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/IResponseOperation.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/regular/RegularHTTPProtocol.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/regular/TemplateProcessor.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/regular/ITemplateDataRetriever.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/regular/responses/ErrorResponse.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/regular/responses/FileContentResponse.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/regular/responses/StringResponse.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/websocket/WebsocketProtocol.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/http/websocket/WebsocketServer.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/event/BaseEvent.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/event/ConnectEvent.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/event/DisconnectEvent.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/ConnAck.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/Connect.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/Disconnect.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/IPacketReceiver.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/MQTTProtocol.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/PacketDecoder.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/PacketIdentifierFactory.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/PingReq.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/PingResp.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/PubAck.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/PubComp.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/Publish.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/PubRec.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/PubRel.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/SubAck.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/Subscribe.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/UnsubAck.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/Unsubscribe.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/packet/MQTTPacket.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/state/ConnectedState.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/state/ConnectToBrokerState.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/state/DisconnectedState.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/state/DisconnectState.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/state/IdleState.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/state/MQTTBaseState.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/state/MqttFSM.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/state/MqttFsmConstants.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/state/RunState.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/state/StartupState.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/IMqttClient.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/InFlight.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/Logging.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/MqttClient.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/MQTTProtocolDefinitions.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/Publication.h
        ${SMOOTH_LIB_ROOT}/include/smooth/application/network/mqtt/Subscription.h
        ${SMOOTH_LIB_ROOT}/include/smooth/core/SystemStatistics.h
        $ENV{IDF_PATH}/components/json/cJSON/cJSON.c
        $ENV{IDF_PATH}/components/json/cJSON/cJSON.h
        )

if (${ESP_PLATFORM})
    list(APPEND SOURCES
            ${SMOOTH_LIB_ROOT}/include/smooth/core/network/Wifi.h
            ${SMOOTH_LIB_ROOT}/core/logging/idf/idf_log.cpp
            ${SMOOTH_LIB_ROOT}/core/network/Wifi.cpp
            ${SMOOTH_LIB_ROOT}/core/io/Input.cpp
            ${SMOOTH_LIB_ROOT}/core/io/Output.cpp
            ${SMOOTH_LIB_ROOT}/core/io/InterruptInput.cpp
            ${SMOOTH_LIB_ROOT}/core/io/i2c/I2CCommandLink.cpp
            ${SMOOTH_LIB_ROOT}/core/io/i2c/I2CMasterDevice.cpp
            ${SMOOTH_LIB_ROOT}/core/io/i2c/Master.cpp
            ${SMOOTH_LIB_ROOT}/core/filesystem/SPIFlash.cpp
            ${SMOOTH_LIB_ROOT}/core/filesystem/SDCard.cpp
            ${SMOOTH_LIB_ROOT}/core/filesystem/SPISDCard.cpp
            ${SMOOTH_LIB_ROOT}/include/smooth/core/filesystem/SPIFlash.h
            ${SMOOTH_LIB_ROOT}/include/smooth/core/filesystem/SDCard.h
            ${SMOOTH_LIB_ROOT}/include/smooth/core/filesystem/SPISDCard.h
            ${SMOOTH_LIB_ROOT}/core/sntp/Sntp.cpp
            ${SMOOTH_LIB_ROOT}/include/smooth/core/sntp/Sntp.h
            ${SMOOTH_LIB_ROOT}/core/filesystem/MMCSDCard.cpp
            ${SMOOTH_LIB_ROOT}/include/smooth/core/filesystem/MMCSDCard.h
            ${SMOOTH_LIB_ROOT}/include/smooth/core/io/Input.h
            ${SMOOTH_LIB_ROOT}/include/smooth/core/io/Output.h
            ${SMOOTH_LIB_ROOT}/include/smooth/core/io/i2c/I2CCommandLink.h
            ${SMOOTH_LIB_ROOT}/include/smooth/core/io/InterruptInput.h
            ${SMOOTH_LIB_ROOT}/include/smooth/core/io/i2c/I2CMasterDevice.h
            ${SMOOTH_LIB_ROOT}/include/smooth/core/io/i2c/Master.h
            ${SMOOTH_LIB_ROOT}/application/io/i2c/MCP23017.cpp
            ${SMOOTH_LIB_ROOT}/application/io/i2c/ADS1115.cpp
            ${SMOOTH_LIB_ROOT}/application/io/i2c/MCP23017.cpp
            ${SMOOTH_LIB_ROOT}/application/io/i2c/BME280.cpp
            ${SMOOTH_LIB_ROOT}/application/io/wiegand/Wiegand.cpp
            ${SMOOTH_LIB_ROOT}/application/security/PasswordHash.cpp
            ${SMOOTH_LIB_ROOT}/include/smooth/application/security/PasswordHash.h
            )
else()
    list(APPEND SOURCES
            ${SMOOTH_LIB_ROOT}/core/logging/posix/posix_log.cpp
            )
endif()

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS ${SMOOTH_LIB_ROOT}/smooth/include
                                    $ENV{IDF_PATH}/components
                                    ${CMAKE_CURRENT_LIST_DIR}/../lib/smooth/include
                        REQUIRES
                            #idf::mbedtls
                            idf::cxx
                            idf::newlib
                            idf::freertos
                            idf::esp_wifi
                            idf::esp32
                            idf::freertos
                            idf::esptool_py
                            idf::fatfs
                            idf::sdmmc
                            idf::spi_flash
                            idf::nvs_flash
                            idf::wear_levelling
                            idf::lwip
                            idf::json
                            idf::xtensa
                            idf::pthread
                            idf::libsodium
                            idf::json
                        )
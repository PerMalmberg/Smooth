project(smooth)

include(${CMAKE_CURRENT_LIST_DIR}/idf_utils.cmake)

smooth_verify_idf_path()

set(CMAKE_CXX_STANDARD 14)
add_definitions(-D_GLIBCXX_USE_C99) # for std::to_string (until using xtensa-gcc 8)

set(SOURCES
        application/network/mqtt/packet/ConnAck.cpp
        application/network/mqtt/packet/Connect.cpp
        application/network/mqtt/packet/Disconnect.cpp
        application/network/mqtt/packet/MQTTPacket.cpp
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
        core/logging/posix/posix_log.cpp
        core/network/IPv4.cpp
        #core/network/IPv6.cpp
        core/network/SocketDispatcher.cpp
        core/timer/ElapsedTime.cpp
        core/timer/Timer.cpp
        core/timer/TimerService.cpp
        core/Application.cpp
        core/json/Value.cpp
        core/json/JsonFile.cpp
        core/Task.cpp
        include/smooth/application/display/ST7735.h
        include/smooth/application/io/ADS1115.h
        include/smooth/application/io/MCP23017.h
        include/smooth/application/network/mqtt/event/BaseEvent.h
        include/smooth/application/network/mqtt/event/ConnectEvent.h
        include/smooth/application/network/mqtt/event/DisconnectEvent.h
        include/smooth/application/network/mqtt/packet/ConnAck.h
        include/smooth/application/network/mqtt/packet/Connect.h
        include/smooth/application/network/mqtt/packet/Disconnect.h
        include/smooth/application/network/mqtt/packet/IPacketReceiver.h
        include/smooth/application/network/mqtt/packet/MQTTPacket.h
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
        include/smooth/core/fsm/StaticFSM.h
        include/smooth/core/ipc/IEventListener.h
        include/smooth/core/ipc/ITaskEventQueue.h
        include/smooth/core/ipc/Link.h
        include/smooth/core/ipc/Publisher.h
        include/smooth/core/ipc/Queue.h
        include/smooth/core/ipc/QueueNotification.h
        include/smooth/core/ipc/SubscribingTaskEventQueue.h
        include/smooth/core/ipc/TaskEventQueue.h
        include/smooth/core/logging/log.h
        include/smooth/core/network/ConnectionStatusEvent.h
        include/smooth/core/network/DataAvailableEvent.h
        include/smooth/core/network/InetAddress.h
        include/smooth/core/network/IPacketAssembly.h
        include/smooth/core/network/IPacketDisassembly.h
        include/smooth/core/network/IPacketReceiveBuffer.h
        include/smooth/core/network/IPacketSendBuffer.h
        include/smooth/core/network/IPv4.h
        include/smooth/core/network/IPv6.h
        include/smooth/core/network/ISocket.h
        include/smooth/core/network/NetworkStatus.h
        include/smooth/core/network/SocketOperation.h
        include/smooth/core/network/PacketReceiveBuffer.h
        include/smooth/core/network/PacketSendBuffer.h
        include/smooth/core/network/Socket.h
        include/smooth/core/network/SocketDispatcher.h
        include/smooth/core/network/TransmitBufferEmptyEvent.h
        include/smooth/core/timer/ElapsedTime.h
        include/smooth/core/timer/ITimer.h
        include/smooth/core/timer/Timer.h
        include/smooth/core/timer/TimerExpiredEvent.h
        include/smooth/core/timer/TimerService.h
        include/smooth/core/util/advance_iterator.h
        include/smooth/core/util/ByteSet.h
        include/smooth/core/util/CircularBuffer.h
        include/smooth/core/util/FixedBuffer.h
        include/smooth/core/util/FixedBufferBase.h
        include/smooth/core/Application.h
        include/smooth/core/Task.h
        include/smooth/core/task_priorities.h
        include/smooth/core/ipc/ILinkSubscriber.h
        include/smooth/core/ipc/IPolledTaskQueue.h
        include/smooth/core/json/Value.h
        include/smooth/core/json/JsonFile.h
        $ENV{IDF_PATH}/components/json/cJSON/cJSON.c
        $ENV{IDF_PATH}/components/json/cJSON/cJSON.h
        )

#if (${ESP_PLATFORM})
    list(APPEND SOURCES
            include/smooth/core/network/Wifi.h
            core/network/Wifi.cpp
            core/io/Input.cpp
            core/io/Output.cpp
            core/io/InterruptInput.cpp
            core/io/i2c/I2CCommandLink.cpp
            core/io/i2c/I2CMasterDevice.cpp
            core/io/i2c/Master.cpp
            core/filesystem/File.cpp
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
            application/io/MCP23017.cpp
            application/io/ADS1115.cpp
            application/io/MCP23017.cpp
            application/sensor/BME280.cpp
            )
#endif ()

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
        PRIVATE $ENV{IDF_PATH}/components
)

smooth_configure_link()



cmake_minimum_required(VERSION 3.8)
project(Smooth)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=bounds -fsanitize=leak -fsanitize=return -fsanitize=null")

include_directories(include)

add_definitions("-DCONFIG_SMOOTH_MAX_MQTT_MESSAGE_SIZE=3500")
add_definitions("-DCONFIG_SMOOTH_MAX_MQTT_OUTGOING_MESSAGES=10")

set(SOURCE_FILES
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
        include/smooth/core/util/make_unique.h
        include/smooth/core/Application.h
        include/smooth/core/Task.h
        include/smooth/core/TaskStatus.h
        include/smooth/core/task_priorities.h
        include/smooth/core/ipc/ILinkSubscriber.h
        include/smooth/core/ipc/IPolledTaskQueue.h

        ${IDF_PATH}/components/json/cJSON/cJSON.c
        ${IDF_PATH}/components/json/cJSON/cJSON.h
        include/smooth/core/json/Value.h
        core/json/Value.cpp)

add_library(${PROJECT_NAME} ${SOURCE_FILES})

set(COMPONENTS
        freertos
        cxx
        esp32
        driver
        log
        tcpip_adapter
        lwip
        openssl
        fatfs/src
        vfs
        storage
        wear_levelling
        nvs_flash
        json/cJSON
        spi_flash
        )

set(COMPONENT_INCLUDE_DIRS "")

foreach (COMP ${COMPONENTS})
    if(EXISTS ${IDF_PATH}/components/${COMP}/include)
        list(APPEND COMPONENT_INCLUDE_DIRS ${IDF_PATH}/components/${COMP}/include)
    else()
        list(APPEND COMPONENT_INCLUDE_DIRS ${IDF_PATH}/components/${COMP})
    endif()
endforeach ()

include_directories(${COMPONENT_INCLUDE_DIRS})

set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${COMPONENT_INCLUDE_DIRS})
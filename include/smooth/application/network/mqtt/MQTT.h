//
// Created by permal on 7/20/17.
//

#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <deque>
#include <unordered_map>
#include <smooth/core/Task.h>
#include <smooth/core/ipc/Mutex.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/network/DataAvailableEvent.h>
#include <smooth/core/network/ConnectionStatusEvent.h>
#include <smooth/core/network/TransmitBufferEmptyEvent.h>
#include <smooth/core/network/Socket.h>
#include <smooth/core/network/SSLSocket.h>
#include <smooth/core/network/PacketSendBuffer.h>
#include <smooth/core/network/PacketReceiveBuffer.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/application/network/mqtt/MQTTPacket.h>
#include <smooth/core/timer/Timer.h>
#include <smooth/core/fsm/StaticFSM.h>
#include <smooth/application/network/mqtt/state/MQTTBaseState.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                // The MQTT class handles everything required for a connection to a single MQTT broker,
                // such as connecting, subscribing and publish topics.

                class MQTT
                        : public smooth::core::Task,
                          public core::ipc::IEventListener<core::network::TransmitBufferEmptyEvent>,
                          public core::ipc::IEventListener<core::network::DataAvailableEvent<mqtt::MQTTPacket>>,
                          public core::ipc::IEventListener<core::network::ConnectionStatusEvent>,
                          public core::ipc::IEventListener<core::timer::TimerExpiredEvent>
                {
                    public:
                        MQTT(const std::string& mqtt_client_id, uint32_t stack_depth, UBaseType_t priority);

                        void connect_to(std::shared_ptr<smooth::core::network::InetAddress> address, bool auto_reconnect, bool use_ssl);
                        void disconnect();

                        void init() override;

                        void subscribe(const std::string& topic);
                        void unsubscribe(const std::string& topic);
                        void publish(const std::string& topic, std::unique_ptr<uint8_t[]> payload, uint16_t length);
                        void publish(const std::string& topic, const std::string& value);

                        void message(const core::network::TransmitBufferEmptyEvent& msg);
                        void message(const core::network::ConnectionStatusEvent& msg);
                        void message(const mqtt::MQTTPacket& msg);
                        void message(const core::network::DataAvailableEvent<mqtt::MQTTPacket>& msg);
                        void message(const core::timer::TimerExpiredEvent& msg);

                    protected:
                        void tick() override;

                    private:

                        void send_packet( MQTTPacket& packet, std::chrono::milliseconds timeout);

                        core::network::PacketSendBuffer<mqtt::MQTTPacket, 5> tx_buffer;
                        core::network::PacketReceiveBuffer<mqtt::MQTTPacket, 5> rx_buffer;
                        core::ipc::TaskEventQueue<core::network::TransmitBufferEmptyEvent> tx_empty;
                        core::ipc::TaskEventQueue<core::network::DataAvailableEvent<mqtt::MQTTPacket>> data_available;
                        core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent> connection_status;
                        core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent> timer_events;
                        std::unordered_map<std::string, std::unique_ptr<uint8_t>> subscriptions;
                        std::deque<std::string> to_unsubscribe;
                        std::unordered_map<std::string, std::pair<std::unique_ptr<uint8_t[]>, uint16_t>> to_publish;
                        smooth::core::ipc::Mutex guard;
                        std::string client_id;
                        std::unique_ptr<smooth::core::network::ISocket> mqtt_socket;
                        core::timer::Timer receive_timer;
                        core::timer::Timer reconnect_timer;
                        smooth::application::network::mqtt::state::MqttFSM fsm;
                        bool auto_reconnect = false;
                };
            }
        }
    }
}
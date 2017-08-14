//
// Created by permal on 7/20/17.
//

#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <deque>
#include <unordered_map>
#include "esp_event.h"
#include <smooth/core/Task.h>
#include <smooth/core/ipc/Mutex.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/network/DataAvailableEvent.h>
#include <smooth/core/network/ConnectionStatusEvent.h>
#include <smooth/core/network/TransmitBufferEmptyEvent.h>
#include <smooth/core/network/Socket.h>
#include <smooth/core/network/PacketSendBuffer.h>
#include <smooth/core/network/PacketReceiveBuffer.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/application/network/mqtt/packet/MQTTPacket.h>
#include <smooth/core/timer/Timer.h>
#include <smooth/application/network/mqtt/state/MqttFSM.h>
#include <smooth/application/network/mqtt/state/MQTTBaseState.h>
#include <smooth/application/network/mqtt/IMqtt.h>
#include <smooth/application/network/mqtt/event/BaseEvent.h>
#include <smooth/application/network/mqtt/Publication.h>
#include <smooth/application/network/mqtt/Subscription.h>

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
                          public IMqtt,
                          public core::ipc::IEventListener<core::network::TransmitBufferEmptyEvent>,
                          public core::ipc::IEventListener<core::network::DataAvailableEvent<packet::MQTTPacket>>,
                          public core::ipc::IEventListener<core::network::ConnectionStatusEvent>,
                          public core::ipc::IEventListener<core::timer::TimerExpiredEvent>,
                          public core::ipc::IEventListener<event::BaseEvent>,
                          public core::ipc::IEventListener<system_event_t>
                {
                    public:
                        MQTT(const std::string& mqtt_client_id, std::chrono::seconds keep_alive, uint32_t stack_depth,
                             UBaseType_t priority);

                        void
                        connect_to(std::shared_ptr<smooth::core::network::InetAddress> address, bool auto_reconnect);
                        void disconnect();
                        void publish(const std::string& topic, const std::string& msg, mqtt::QoS qos, bool retain);
                        void publish(const std::string& topic, const uint8_t* data, int length, mqtt::QoS qos, bool retain);

                        void subscribe(const std::string topic);

                        void init() override;

                        void event(const core::network::TransmitBufferEmptyEvent& event);
                        void event(const core::network::ConnectionStatusEvent& event);
                        void event(const packet::MQTTPacket& event);
                        void event(const core::network::DataAvailableEvent<packet::MQTTPacket>& event);
                        void event(const core::timer::TimerExpiredEvent& event);

                        void event(const event::BaseEvent& event);

                        void event(const system_event_t& event);

                        const std::string& get_client_id() const override;
                        const std::chrono::seconds get_keep_alive() const override;
                        void start_reconnect() override;
                        void reconnect() override;
                        void set_keep_alive_timer(std::chrono::seconds interval) override;

                        bool is_auto_reconnect() const override
                        {
                            return auto_reconnect;
                        }

                        Publication& get_publication()
                        {
                            return publication;
                        }

                        Subscription& get_subscription()
                        {
                            return subscription;
                        }

                    protected:
                        void tick() override;

                    private:

                        bool send_packet(packet::MQTTPacket& packet) override;

                        core::network::PacketSendBuffer<packet::MQTTPacket, 5> tx_buffer;
                        core::network::PacketReceiveBuffer<packet::MQTTPacket, 5> rx_buffer;
                        core::ipc::TaskEventQueue<core::network::TransmitBufferEmptyEvent> tx_empty;
                        core::ipc::TaskEventQueue<core::network::DataAvailableEvent<packet::MQTTPacket>> data_available;
                        core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent> connection_status;
                        core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent> timer_events;
                        core::ipc::TaskEventQueue<smooth::application::network::mqtt::event::BaseEvent> control_event;
                        core::ipc::SubscribingTaskEventQueue<system_event_t> system_event;
                        smooth::core::ipc::Mutex guard;
                        std::string client_id;
                        std::chrono::seconds keep_alive;
                        std::shared_ptr<smooth::core::network::ISocket> mqtt_socket;
                        core::timer::Timer reconnect_timer;
                        core::timer::Timer keep_alive_timer;
                        smooth::application::network::mqtt::state::MqttFSM<state::MQTTBaseState> fsm;
                        bool auto_reconnect = false;
                        std::shared_ptr<smooth::core::network::InetAddress> address;
                        Publication publication{};
                        Subscription subscription{};

                };
            }
        }
    }
}
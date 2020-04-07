/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <chrono>
#include <memory>
#include <deque>
#include <mutex>
#include <unordered_map>
#include "smooth/core/Task.h"
#include "smooth/core/network/IPv4.h"
#include "smooth/core/network/event/DataAvailableEvent.h"
#include "smooth/core/network/event/ConnectionStatusEvent.h"
#include "smooth/core/network/event/TransmitBufferEmptyEvent.h"
#include "smooth/core/network/Socket.h"
#include "smooth/core/network/PacketSendBuffer.h"
#include "smooth/core/network/PacketReceiveBuffer.h"
#include "smooth/core/network/NetworkStatus.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/ipc/SubscribingTaskEventQueue.h"
#include "smooth/application/network/mqtt/packet/MQTTProtocol.h"
#include "smooth/core/timer/Timer.h"
#include "smooth/application/network/mqtt/state/MqttFSM.h"
#include "smooth/application/network/mqtt/state/MQTTBaseState.h"
#include "smooth/application/network/mqtt/IMqttClient.h"
#include "smooth/application/network/mqtt/event/BaseEvent.h"
#include "smooth/application/network/mqtt/Publication.h"
#include "smooth/application/network/mqtt/Subscription.h"
#include "smooth/core/network/BufferContainer.h"

namespace smooth::application::network::mqtt
{
    typedef std::pair<std::string, std::vector<uint8_t>> MQTTData;

    /// MQTT client; handles everything required for a connection to a single MQTT broker
    /// such as connecting, subscribing and publishing topics.
    class MqttClient
        : public smooth::core::Task,
        private IMqttClient,
        public core::ipc::IEventListener<core::network::event::TransmitBufferEmptyEvent>,
        public core::ipc::IEventListener<core::network::event::DataAvailableEvent<packet::MQTTProtocol>>,
        public core::ipc::IEventListener<core::network::event::ConnectionStatusEvent>,
        private core::ipc::IEventListener<core::timer::TimerExpiredEvent>,
        private core::ipc::IEventListener<event::BaseEvent>,
        private core::ipc::IEventListener<smooth::core::network::NetworkStatus>,
        public std::enable_shared_from_this<MqttClient>
    {
        public:
            /// Constructor
            /// \param mqtt_client_id The client ID to use when identifying to the broker. Must be unique.
            /// \param keep_alive The interval used for keep alive messages.
            /// \param stack_size The stack depth for the worker task. >=4096 should be sufficient.
            /// \param priority Task priority. Depends on your system requirements. Usually tskIDLE_PRIORITY + some
            // value.
            /// \param application_queue The queue where incoming messages will be posted.
            MqttClient(const std::string& mqtt_client_id, std::chrono::seconds keep_alive,
                       uint32_t stack_size,
                       uint32_t priority, std::weak_ptr<core::ipc::TaskEventQueue<MQTTData>> application_queue);

            /// Initiates a connection to the provided address.
            /// \param server_address The address
            /// \param enable_auto_reconnect If true, the client will automatically reconnect when connection is lost.
            void
            connect_to(const std::shared_ptr<smooth::core::network::InetAddress>& server_address,
                       bool enable_auto_reconnect);

            void reconnect() override
            {
                if (address)
                {
                    connect_to(address, is_auto_reconnect());
                }
            }

            /// Disconnects from the broker.
            void disconnect() override;

            /// Publishes a message.
            /// Note: There is a maximum number of messages that can be in the outgoing queue. This number
            /// is determined by the configuration at compile time.
            /// \param topic The topic.
            /// \param msg The message
            /// \param qos The QoS level to publish the message as.
            /// \param retain if true, the message is marked for retainment in the broker.
            /// \return true if the message could be queued for delivery, otherwise false. A true value
            /// does not mean it has been delivered.
            bool publish(const std::string& topic, const std::string& msg, mqtt::QoS qos, bool retain);

            /// Publishes a message.
            /// Note: There is a maximum number of messages that can be in the outgoing queue. This number
            /// is determined by the configuration at compile time
            /// \param topic The topic.
            /// \param data The data, as an array of bytes.
            /// \param length The length of the array pointed to by the data parameter.
            /// \param qos The QoS level to publish the message as.
            /// \param retain if true, the message is marked for retainment in the broker.
            /// \return true if the message could be queued for delivery, otherwise false. A true value
            /// does not mean it has been delivered.
            bool
            publish(const std::string& topic, const uint8_t* data, int length, mqtt::QoS qos, bool retain);

            /// Subscribes to a topic.
            /// \param topic The topic
            /// \param qos The QoS to use for subscription.
            void subscribe(const std::string& topic, QoS qos);

            /// Unsubscribes from a topic.
            /// \param topic The topic.
            void unsubscribe(const std::string& topic);

            /// Returns a value indicating of the client is connected on a socket-level or not
            /// \return true or false
            bool is_connected() const
            {
                return connected;
            }

            static std::string get_payload(const MQTTData& data);

        private:
            void event(const core::network::event::TransmitBufferEmptyEvent& event) override;

            void event(const core::network::event::ConnectionStatusEvent& event) override;

            void event(const core::network::event::DataAvailableEvent<packet::MQTTProtocol>& event) override;

            void event(const core::timer::TimerExpiredEvent& event) override;

            void event(const event::BaseEvent& event) override;

            void event(const smooth::core::network::NetworkStatus& event) override;

            const std::string& get_client_id() const override;

            std::chrono::seconds get_keep_alive() const override;

            void start_reconnect() override;

            void set_keep_alive_timer(std::chrono::seconds interval) override;

            bool is_auto_reconnect() const override
            {
                return auto_reconnect;
            }

            Publication& get_publication() override
            {
                return publication;
            }

            Subscription& get_subscription() override
            {
                return subscription;
            }

            std::weak_ptr<core::ipc::TaskEventQueue<std::pair<std::string,
                                                              std::vector<uint8_t>>>> get_application_queue() override
            {
                return application_queue;
            }

            void init() override;

            void tick() override;

            bool send_packet(packet::MQTTProtocol::packet_type& packet) override;

            void force_disconnect() override;

            using ControlQueue = core::ipc::TaskEventQueue<smooth::application::network::mqtt::event::BaseEvent>;
            using SystemQueue = core::ipc::SubscribingTaskEventQueue<smooth::core::network::NetworkStatus>;
            using TimerQueue = core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent>;

            std::weak_ptr<core::ipc::TaskEventQueue<std::pair<std::string, std::vector<uint8_t>>>> application_queue;
            std::shared_ptr<TimerQueue> timer_events;
            std::shared_ptr<ControlQueue> control_event;
            std::shared_ptr<SystemQueue> system_event;
            std::mutex guard;
            std::string client_id;
            std::chrono::seconds keep_alive;
            std::shared_ptr<smooth::core::network::ISocket> mqtt_socket;
            core::timer::TimerOwner reconnect_timer;
            core::timer::TimerOwner keep_alive_timer;
            smooth::application::network::mqtt::state::MqttFSM<state::MQTTBaseState> fsm;
            bool auto_reconnect = false;
            std::shared_ptr<smooth::core::network::InetAddress> address;
            Publication publication{};
            Subscription subscription{};
            bool connected = false;
            std::mutex address_guard{};
            std::shared_ptr<smooth::core::network::BufferContainer<packet::MQTTProtocol>> buff{};
    };
}

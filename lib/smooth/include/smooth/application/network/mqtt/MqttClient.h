//
// Created by permal on 7/20/17.
//

#pragma once

#include <chrono>
#include <memory>
#include <deque>
#include <mutex>
#include <unordered_map>
#include <smooth/core/Task.h>
#include <smooth/core/network/IPv4.h>
#include <smooth/core/network/DataAvailableEvent.h>
#include <smooth/core/network/ConnectionStatusEvent.h>
#include <smooth/core/network/TransmitBufferEmptyEvent.h>
#include <smooth/core/network/Socket.h>
#include <smooth/core/network/PacketSendBuffer.h>
#include <smooth/core/network/PacketReceiveBuffer.h>
#include <smooth/core/network/NetworkStatus.h>
#include <smooth/core/ipc/TaskEventQueue.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/application/network/mqtt/packet/MQTTProtocol.h>
#include <smooth/core/timer/Timer.h>
#include <smooth/application/network/mqtt/state/MqttFSM.h>
#include <smooth/application/network/mqtt/state/MQTTBaseState.h>
#include <smooth/application/network/mqtt/IMqttClient.h>
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
                typedef std::pair<std::string, std::vector<uint8_t>> MQTTData;

                /// MQTT client; handles everything required for a connection to a single MQTT broker
                /// such as connecting, subscribing and publishing topics.
                class MqttClient
                        : private smooth::core::Task,
                          private IMqttClient,
                          private core::ipc::IEventListener<core::network::TransmitBufferEmptyEvent>,
                          private core::ipc::IEventListener<core::network::DataAvailableEvent<packet::MQTTProtocol>>,
                          private core::ipc::IEventListener<core::network::ConnectionStatusEvent>,
                          private core::ipc::IEventListener<core::timer::TimerExpiredEvent>,
                          private core::ipc::IEventListener<event::BaseEvent>,
                          private core::ipc::IEventListener<smooth::core::network::NetworkStatus>
                {
                    public:
                        /// Constructor
                        /// \param mqtt_client_id The client ID to use when identifying to the broker. Must be unique.
                        /// \param keep_alive The interval used for keep alive messages.
                        /// \param stack_size The stack depth for the worker task. >=4096 should be sufficient.
                        /// \param priority Task priority. Depends on your system requirements. Usually tskIDLE_PRIORITY + some value.
                        /// \param application_queue The queue where incoming messages will be posted.
                        MqttClient(const std::string& mqtt_client_id, std::chrono::seconds keep_alive,
                                   uint32_t stack_size,
                                   uint32_t priority, core::ipc::TaskEventQueue<MQTTData>& application_queue);

                        /// Initiates a connection to the provided address.
                        /// \param address The address
                        /// \param auto_reconnect If true, the client will automatically reconnect when connection is lost.
                        void
                        connect_to(std::shared_ptr<smooth::core::network::InetAddress> address, bool auto_reconnect);

                        void reconnect() override
                        {
                            if(address)
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
                        void event(const core::network::TransmitBufferEmptyEvent& event) override;
                        void event(const core::network::ConnectionStatusEvent& event) override;
                        void event(const core::network::DataAvailableEvent<packet::MQTTProtocol>& event) override;

                        void event(const core::timer::TimerExpiredEvent& event) override;

                        void event(const event::BaseEvent& event) override;

                        void event(const smooth::core::network::NetworkStatus& event) override;
                        const std::string& get_client_id() const override;
                        const std::chrono::seconds get_keep_alive() const override;
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

                        core::ipc::TaskEventQueue<std::pair<std::string, std::vector<uint8_t>>>& get_application_queue() override
                        {
                            return application_queue;
                        }

                        void init() override;
                        void tick() override;

                        bool send_packet(packet::MQTTProtocol& packet) override;
                        void force_disconnect() override;

                        core::ipc::TaskEventQueue<std::pair<std::string, std::vector<uint8_t>>>& application_queue;
                        core::network::PacketSendBuffer<packet::MQTTProtocol, 5> tx_buffer{};
                        core::network::PacketReceiveBuffer<packet::MQTTProtocol, 5> rx_buffer{};
                        core::ipc::TaskEventQueue<core::network::TransmitBufferEmptyEvent> tx_empty;
                        core::ipc::TaskEventQueue<core::network::DataAvailableEvent<packet::MQTTProtocol>> data_available;
                        core::ipc::TaskEventQueue<smooth::core::network::ConnectionStatusEvent> connection_status;
                        core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent> timer_events;
                        core::ipc::TaskEventQueue<smooth::application::network::mqtt::event::BaseEvent> control_event;
                        core::ipc::SubscribingTaskEventQueue<smooth::core::network::NetworkStatus> system_event;
                        std::mutex guard;
                        std::string client_id;
                        std::chrono::seconds keep_alive;
                        std::shared_ptr<smooth::core::network::ISocket> mqtt_socket;
                        std::shared_ptr<core::timer::Timer> reconnect_timer;
                        std::shared_ptr<core::timer::Timer> keep_alive_timer;
                        smooth::application::network::mqtt::state::MqttFSM<state::MQTTBaseState> fsm;
                        bool auto_reconnect = false;
                        std::shared_ptr<smooth::core::network::InetAddress> address;
                        Publication publication{};
                        Subscription subscription{};
                        bool connected = false;
                        std::mutex address_guard{};
                };
            }
        }
    }
}
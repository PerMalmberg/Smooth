//
// Created by permal on 7/20/17.
//

#include <smooth/application/network/mqtt/MQTT.h>
#include <smooth/application/network/mqtt/Connect.h>
#include <smooth/application/network/mqtt/state/ConnectingState.h>
#include "esp_log.h"


using namespace smooth::core::ipc;
using namespace std::chrono;

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                MQTT::MQTT(const std::string& mqtt_client_id, uint32_t stack_depth, UBaseType_t priority)
                        : Task(mqtt_client_id, stack_depth, priority, std::chrono::milliseconds(100)),
                          tx_buffer(),
                          rx_buffer(),
                          tx_empty("TX_empty", 5, *this, *this),
                          data_available("data_available", 5, *this, *this),
                          connection_status("connection_status", 5, *this, *this),
                          timer_events("timer_events", 5, *this, *this),
                          subscriptions(),
                          to_unsubscribe(),
                          to_publish(),
                          guard(),
                          client_id(mqtt_client_id),
                          mqtt_socket(nullptr),
                          receive_timer("receive_timer", 1, timer_events, false, std::chrono::seconds(10)),
                          reconnect_timer("reconnect_timer", 2, timer_events, false, std::chrono::seconds(5)),
                          fsm()
                {
                }

                void MQTT::tick()
                {
                }


                void MQTT::init()
                {
                    fsm.set_state(new(fsm) state::ConnectingState(fsm));
                }

                void MQTT::connect_to(std::shared_ptr<smooth::core::network::InetAddress> address, bool auto_reconnect,
                                      bool use_ssl)
                {
                    Mutex::Lock lock(guard);

                    this->auto_reconnect = auto_reconnect;

                    if (!mqtt_socket)
                    {
                        if (use_ssl)
                        {
                            mqtt_socket.reset(
                                    new core::network::SSLSocket<mqtt::MQTTPacket>(tx_buffer,
                                                                                   rx_buffer,
                                                                                   tx_empty,
                                                                                   data_available,
                                                                                   connection_status));
                        }
                        else
                        {
                            mqtt_socket.reset(
                                    new core::network::Socket<mqtt::MQTTPacket>(tx_buffer,
                                                                                rx_buffer,
                                                                                tx_empty,
                                                                                data_available,
                                                                                connection_status));
                        }

                        mqtt_socket->start(address);
                    }
                }

                void MQTT::disconnect()
                {
                    auto_reconnect = false;
                    mqtt_socket->stop();
                    mqtt_socket.reset();
                }

                void MQTT::send_packet(MQTTPacket& packet, milliseconds timeout)
                {
                    receive_timer.start(timeout);
                    tx_buffer.put(packet);
                }

                void MQTT::message(const core::network::TransmitBufferEmptyEvent& msg)
                {
                    ESP_LOGD("MQTT", "TransmitBufferEmptyEvent");
                }

                void MQTT::message(const core::network::ConnectionStatusEvent& msg)
                {
                    ESP_LOGD("MQTT", "ConnectionStatusEvent: %d", msg.is_connected());
                    if (msg.is_connected())
                    {
                        reconnect_timer.stop();
                        Connect con(client_id.substr(0, 23));
                        send_packet(con, milliseconds(500));
                    }
                    else if (auto_reconnect)
                    {
                        reconnect_timer.start();
                    }
                }

                void MQTT::message(const core::network::DataAvailableEvent<mqtt::MQTTPacket>& msg)
                {
                    receive_timer.stop();
                    MQTTPacket p;
                    msg.get(p);
                    ESP_LOGD("MQTT", "DataAvailableEvent");
                    fsm.set_state(new(fsm) state::ConnectingState(fsm));
                }

                void MQTT::message(const core::timer::TimerExpiredEvent& msg)
                {
                    if (msg.get_timer() == &receive_timer)
                    {
                        ESP_LOGV("MQTT", "receive_timer");
                        // Timeout while receiving data, shutdown socket.
                        mqtt_socket->restart();
                    }
                    else if (msg.get_timer() == &reconnect_timer)
                    {
                        ESP_LOGV("MQTT", "reconnect_timer");
                        mqtt_socket->restart();
                    }
                }

                void MQTT::subscribe(const std::string& topic)
                {
                    Mutex::Lock lock(guard);
                    subscriptions.emplace(topic, nullptr);
                }

                void MQTT::unsubscribe(const std::string& topic)
                {
                    Mutex::Lock lock(guard);
                    auto item = subscriptions.find(topic);
                    if (item != subscriptions.end())
                    {
                        to_unsubscribe.push_back(item->first);
                        subscriptions.erase(item);
                    }
                }

                void MQTT::publish(const std::string& topic, std::unique_ptr<uint8_t[]> payload, uint16_t length)
                {
                    Mutex::Lock lock(guard);
                    auto item = to_publish.find(topic);
                    if (item == to_publish.end())
                    {
                        to_publish.insert(std::make_pair(topic, std::make_pair(std::move(payload), length)));
                    }
                    else
                    {
                        // Already have an outgoing item, replace it.
                        item->second.first = std::move(payload);
                        item->second.second = length;
                    }
                }

                void MQTT::publish(const std::string& topic, const std::string& value)
                {
                    Mutex::Lock lock(guard);
                    // Include zero terminator
                    uint16_t length = value.length() + 1;
                    auto data = std::unique_ptr<uint8_t[]>(new uint8_t[length]);
                    if (data)
                    {
                        std::memcpy(data.get(), value.c_str(), length);
                        publish(topic, std::move(data), length);
                    }
                }
            }
        }
    }
}
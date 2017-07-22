//
// Created by permal on 7/20/17.
//

#include <smooth/application/network/mqtt/MQTT.h>
#include <smooth/application/network/mqtt/Connect.h>
#include "esp_log.h"


using namespace smooth::core::ipc;

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                MQTT::MQTT(const std::string& mqtt_client_id, uint32_t stack_depth, UBaseType_t priority,
                           std::chrono::milliseconds tick_interval)
                        : Task(mqtt_client_id, stack_depth, priority, tick_interval),
                          tx_buffer(),
                          rx_buffer(),
                          tx_empty("TX_empty", 5, *this, *this),
                          data_available("data_available", 5, *this, *this),
                          connection_status("connection_status", 5, *this, *this),
                          subscriptions(),
                          to_unsubscribe(),
                          to_publish(),
                          guard(),
                          mqtt_socket(nullptr)
                {
                }

                void MQTT::tick()
                {
                }

                void MQTT::connect_to(std::shared_ptr<smooth::core::network::InetAddress> address, bool use_ssl)
                {
                    Mutex::Lock lock(guard);

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

                void MQTT::message(const core::network::TransmitBufferEmptyEvent& msg)
                {
                    ESP_LOGD("MQTT", "TransmitBufferEmptyEvent");
                }

                void MQTT::message(const core::network::ConnectionStatusEvent& msg)
                {
                    ESP_LOGD("MQTT", "ConnectionStatusEvent: %d", msg.is_connected());
                    if( msg.is_connected())
                    {
                        Connect con("HAP-ESP32");
                        con.dump();
                        tx_buffer.put(con);
                    }
                }

                void MQTT::message(const core::network::DataAvailableEvent<mqtt::MQTTPacket>& msg)
                {
                    MQTTPacket p;
                    msg.get(p);
                    ESP_LOGD("MQTT", "DataAvailableEvent");
                    p.dump();
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
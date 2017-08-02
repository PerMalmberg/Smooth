//
// Created by permal on 7/20/17.
//

#include <smooth/application/network/mqtt/MQTT.h>
#include <smooth/application/network/mqtt/packet/Connect.h>
#include <smooth/application/network/mqtt/state/IdleState.h>
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
                MQTT::MQTT(const std::string& mqtt_client_id,
                           std::chrono::seconds keep_alive,
                           uint32_t stack_depth,
                           UBaseType_t priority)
                        : Task(mqtt_client_id, stack_depth, priority, std::chrono::milliseconds(100)),
                          tx_buffer(),
                          rx_buffer(),
                          tx_empty("TX_empty", 5, *this, *this),
                          data_available("data_available", 5, *this, *this),
                          connection_status("connection_status", 5, *this, *this),
                          timer_events("timer_events", 5, *this, *this),
                          guard(),
                          client_id(mqtt_client_id),
                          keep_alive(keep_alive),
                          mqtt_socket(),
                          receive_timer("receive_timer", MQTT_FSM_RECEIVE_TIMER_ID, timer_events, false,
                                        std::chrono::seconds(10)),
                          reconnect_timer("reconnect_timer", MQTT_FSM_RECONNECT_TIMER_ID, timer_events, false,
                                          std::chrono::seconds(5)),
                          keep_alive_timer("keep_alive_timer", MQTT_FSM_KEEP_ALIVE_TIMER_ID, timer_events, true,
                                           std::chrono::seconds(1)),
                          fsm(*this)
                {
                }

                void MQTT::tick()
                {
                    fsm.tick();
                }


                void MQTT::init()
                {
                    fsm.set_state(new(fsm) state::IdleState(fsm));
                }

                void MQTT::connect_to(std::shared_ptr<smooth::core::network::InetAddress> address, bool auto_reconnect,
                                      bool use_ssl)
                {
                    Mutex::Lock lock(guard);

                    this->auto_reconnect = auto_reconnect;

                    if( mqtt_socket)
                    {
                        mqtt_socket->stop(false);
                    }

                    if (use_ssl)
                    {
                        mqtt_socket.reset(
                                new core::network::SSLSocket<packet::MQTTPacket>(tx_buffer,
                                                                                 rx_buffer,
                                                                                 tx_empty,
                                                                                 data_available,
                                                                                 connection_status));
                    }
                    else
                    {
                        mqtt_socket.reset(
                                new core::network::Socket<packet::MQTTPacket>(tx_buffer,
                                                                              rx_buffer,
                                                                              tx_empty,
                                                                              data_available,
                                                                              connection_status));
                    }

                    mqtt_socket->start(address);
                }

                void MQTT::disconnect()
                {
                    fsm.disconnect_event();
                }

                void MQTT::shutdown_connection()
                {
                    auto_reconnect = false;
                    if (mqtt_socket)
                    {
                        mqtt_socket->stop(true);
                    }

                    receive_timer.stop();
                    keep_alive_timer.stop();
                    reconnect_timer.stop();
                }

                void MQTT::send_packet(packet::MQTTPacket& packet, milliseconds timeout)
                {
                    receive_timer.start(timeout);
                    tx_buffer.put(packet);
                }

                const std::string& MQTT::get_client_id() const
                {
                    return client_id;
                }

                const std::chrono::seconds MQTT::get_keep_alive() const
                {
                    return keep_alive;
                }

                void MQTT::start_reconnect()
                {
                    reconnect_timer.start();
                }

                void MQTT::reconnect()
                {
                    if (mqtt_socket)
                    {
                        mqtt_socket->restart();
                    }
                }

                bool MQTT::get_auto_reconnect() const
                {
                    return auto_reconnect;
                }

                void MQTT::set_keep_alive_timer(std::chrono::seconds interval)
                {
                    if (interval.count() == 0)
                    {
                        keep_alive_timer.stop();
                    }
                    else
                    {
                        std::chrono::milliseconds ms = interval;
                        ms /= 2;
                        keep_alive_timer.start(ms);
                    }
                }

                void MQTT::message(const core::network::TransmitBufferEmptyEvent& msg)
                {
                    fsm.message(msg);
                }

                void MQTT::message(const core::network::ConnectionStatusEvent& msg)
                {
                    fsm.message(msg);
                }

                void MQTT::message(const core::network::DataAvailableEvent<packet::MQTTPacket>& msg)
                {
                    receive_timer.stop();

                    packet::MQTTPacket p;
                    if (msg.get(p))
                    {
                        fsm.packet_received(p);
                    }
                }

                void MQTT::message(const core::timer::TimerExpiredEvent& msg)
                {
                    fsm.message(msg);
                }
            }
        }
    }
}
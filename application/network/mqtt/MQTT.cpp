//
// Created by permal on 7/20/17.
//

#include <smooth/application/network/mqtt/MQTT.h>
#include <smooth/application/network/mqtt/packet/Connect.h>
#include <smooth/application/network/mqtt/state/StartupState.h>
#include <smooth/application/network/mqtt/event/ConnectEvent.h>
#include <smooth/application/network/mqtt/event/DisconnectEvent.h>
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
                          control_event("control_event", 5, *this, *this),
                          system_event("system_event", 5, *this, *this),
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
                          fsm(*this),
                          address()
                {
                }

                void MQTT::tick()
                {
                    fsm.tick();
                }


                void MQTT::init()
                {
                    fsm.set_state(new(fsm) state::StartupState(fsm));
                }

                void MQTT::connect_to(std::shared_ptr<smooth::core::network::InetAddress> address, bool auto_reconnect)
                {
                    Mutex::Lock lock(guard);
                    this->address = address;
                    this->auto_reconnect = auto_reconnect;
                    control_event.push(event::ConnectEvent());
                }

                void MQTT::disconnect()
                {
                    control_event.push(event::DisconnectEvent());
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
                    control_event.push(event::DisconnectEvent());
                    control_event.push(event::ConnectEvent());
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

                void MQTT::event(const core::network::TransmitBufferEmptyEvent& event)
                {
                    fsm.event(event);
                }

                void MQTT::event(const core::network::ConnectionStatusEvent& event)
                {
                    fsm.event(event);
                }

                void MQTT::event(const core::network::DataAvailableEvent<packet::MQTTPacket>& event)
                {
                    receive_timer.stop();

                    packet::MQTTPacket p;
                    if (event.get(p))
                    {
                        fsm.packet_received(p);
                    }
                }

                void MQTT::event(const core::timer::TimerExpiredEvent& event)
                {
                    fsm.event(event);
                }

                void MQTT::event(const event::BaseEvent& event)
                {
                    if (event.get_type() == event::BaseEvent::DISCONNECT)
                    {
                        receive_timer.stop();
                        keep_alive_timer.stop();
                        reconnect_timer.stop();
                        fsm.disconnect_event();
                    }
                    else if (event.get_type() == event::BaseEvent::CONNECT)
                    {
                        if (mqtt_socket)
                        {
                            mqtt_socket->stop();
                        }


                        mqtt_socket = core::network::Socket<packet::MQTTPacket>::create(tx_buffer,
                                                                                        rx_buffer,
                                                                                        tx_empty,
                                                                                        data_available,
                                                                                        connection_status);

                        mqtt_socket->start(address);
                    }
                }

                void MQTT::event(const system_event_t& event)
                {
                    if (event.event_id == SYSTEM_EVENT_STA_GOT_IP
                        || event.event_id == SYSTEM_EVENT_AP_STA_GOT_IP6)
                    {
                        reconnect();
                    }
                }
            }
        }
    }
}
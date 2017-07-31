//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/ConnectedState.h>
#include <smooth/application/network/mqtt/state/IdleState.h>
#include <smooth/application/network/mqtt/packet/PingReq.h>

namespace smooth
{
    namespace application
    {
        namespace network
        {
            namespace mqtt
            {
                namespace state
                {
                    void ConnectedState::message(const core::timer::TimerExpiredEvent& msg)
                    {
                        auto timer_id = msg.get_timer()->get_id();

                        if (timer_id == MQTT_FSM_RECEIVE_TIMER_ID)
                        {
                            ESP_LOGV("ConnectedState", "MQTT_FSM_RECEIVE_TIMER_ID")
                            fsm.set_state(new(fsm) IdleState(fsm));
                        }
                        else if (timer_id == MQTT_FSM_KEEP_ALIVE_TIMER_ID)
                        {
                            ESP_LOGV("ConnectedState", "MQTT_FSM_KEEP_ALIVE_TIMER_ID")
                            packet::PingReq ping;
                            fsm.get_mqtt().send_packet(ping, std::chrono::seconds(1));
                        }
                    }

                    void ConnectedState::message(const core::network::ConnectionStatusEvent& msg)
                    {
                        if (!msg.is_connected())
                        {
                            fsm.set_state(new IdleState(fsm, fsm.get_mqtt().get_auto_reconnect()));
                        }
                    }
                }
            }
        }
    }
}
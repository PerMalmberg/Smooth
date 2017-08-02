//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/DisconnectedState.h>

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
                    void DisconnectedState::enter_state()
                    {
                        fsm.get_mqtt().set_keep_alive_timer(std::chrono::seconds(0));
                        if (auto_reconnect)
                        {
                            fsm.get_mqtt().start_reconnect();
                        }
                        else
                        {
                            fsm.get_mqtt().shutdown_connection();
                        }
                    }

                    void DisconnectedState::message(const core::timer::TimerExpiredEvent& msg)
                    {
                        if (msg.get_timer()->get_id() == MQTT_FSM_RECONNECT_TIMER_ID)
                        {
                            fsm.get_mqtt().reconnect();
                        }
                    }

                    void DisconnectedState::message(const core::network::ConnectionStatusEvent& msg)
                    {
                        if (auto_reconnect && !msg.is_connected())
                        {
                            fsm.get_mqtt().start_reconnect();
                        }
                    }

                }
            }
        }
    }
}
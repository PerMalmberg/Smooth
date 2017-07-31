//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/ConnectedState.h>
#include <smooth/application/network/mqtt/state/IdleState.h>

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
                        if (msg.get_timer()->get_id() == MQTT_FSM_RECEIVE_TIMER_ID)
                        {
                            fsm.set_state(new(fsm) IdleState(fsm));
                        }
                        else if( msg.get_timer()->get_id() == MQTT_FSM_KEEP_ALIVE_TIMER_ID)
                        {
                            // QQQ
                        }
                    }

                    void ConnectedState::message(const core::network::ConnectionStatusEvent& msg)
                    {
                        if (!msg.is_connected())
                        {
                            fsm.set_state(new IdleState(fsm));
                        }
                    }
                }
            }
        }
    }
}
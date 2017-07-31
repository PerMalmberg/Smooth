//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/ConnectToBrokerState.h>
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
                    }
                }
            }
        }
    }
}
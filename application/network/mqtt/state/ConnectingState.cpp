//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/ConnectingState.h>
#include <smooth/application/network/mqtt/state/IdleState.h>
#include <smooth/application/network/mqtt/state/SubscribeState.h>
#include <smooth/application/network/mqtt/packet/Connect.h>

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

                    void ConnectingState::EnterState()
                    {
                        packet::Connect con(fsm.get_mqtt().get_client_id()) ;
                        fsm.get_mqtt().send_packet(con, std::chrono::seconds(2));
                    }

                    void ConnectingState::message(const core::timer::TimerExpiredEvent& msg)
                    {
                        if (msg.get_timer()->get_id() == MQTT_FSM_RECEIVE_TIMER_ID)
                        {
                            fsm.set_state(new(fsm) IdleState(fsm));
                        }
                    }

                    void ConnectingState::receive(packet::ConnAck& conn_ack)
                    {
                        fsm.set_state(new(fsm) SubscribeState(fsm));
                    }
                }
            }
        }
    }
}
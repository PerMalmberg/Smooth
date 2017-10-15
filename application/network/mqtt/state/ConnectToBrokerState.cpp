//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/ConnectToBrokerState.h>
#include <smooth/application/network/mqtt/state/RunState.h>
#include <smooth/application/network/mqtt/state/IdleState.h>
#include <smooth/application/network/mqtt/packet/Connect.h>
#include <smooth/application/network/mqtt/packet/ConnAck.h>

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
                    void ConnectToBrokerState::enter_state()
                    {
                        packet::Connect con(fsm.get_mqtt().get_client_id(), fsm.get_mqtt().get_keep_alive());
                        fsm.get_mqtt().send_packet(con);
                        fsm.get_mqtt().set_keep_alive_timer(fsm.get_mqtt().get_keep_alive());
                    }

                    void ConnectToBrokerState::receive(packet::ConnAck& conn_ack)
                    {
                        if (conn_ack.connection_was_accepted())
                        {
                            fsm.set_state(new(fsm) RunState(fsm));
                        }
                        else
                        {
                            fsm.set_state(new(fsm) IdleState(fsm));
                        }
                    }
                }
            }
        }
    }
}
//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/ConnectToBrokerState.h>
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
                    void ConnectToBrokerState::enter_state()
                    {
                        packet::Connect con(fsm.get_mqtt().get_client_id(), fsm.get_mqtt().get_keep_alive());
                        fsm.get_mqtt().send_packet(con, std::chrono::seconds(2));
                        fsm.get_mqtt().set_keep_alive_timer(fsm.get_mqtt().get_keep_alive());
                    }

                    void ConnectToBrokerState::receive(packet::ConnAck& conn_ack)
                    {
                        fsm.set_state(new(fsm) SubscribeState(fsm));
                    }
                }
            }
        }
    }
}
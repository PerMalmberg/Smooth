//
// Created by permal on 7/31/17.
//

#include <smooth/application/network/mqtt/state/IdleState.h>
#include <smooth/application/network/mqtt/state/MqttFsmConstants.h>
#include <smooth/application/network/mqtt/state/ConnectToBrokerState.h>
#include <smooth/application/network/mqtt/state/MqttFSM.h>

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
                    void IdleState::message(const core::network::ConnectionStatusEvent& msg)
                    {
                        if (msg.is_connected())
                        {
                            fsm.set_state(new(fsm) ConnectToBrokerState(fsm));
                        }
                        else
                        {
                            DisconnectedState::message(msg);
                        }
                    }
                }
            }
        }
    }
}
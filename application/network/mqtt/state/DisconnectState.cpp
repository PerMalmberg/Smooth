//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/DisconnectState.h>
#include <smooth/application/network/mqtt/state/IdleState.h>
#include <smooth/application/network/mqtt/packet/Disconnect.h>

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
                    void DisconnectState::enter_state()
                    {
                        packet::Disconnect disconnect;
                        fsm.get_mqtt().send_packet(disconnect, std::chrono::seconds(5));
                    }

                    void DisconnectState::event(const core::network::TransmitBufferEmptyEvent& event)
                    {
                        // Disconnect sent, we're done
                        fsm.set_state(new(fsm) IdleState(fsm));
                    }

                    void DisconnectState::event(const core::timer::TimerExpiredEvent& event)
                    {
                        if( event.get_timer()->get_id() == MQTT_FSM_RECEIVE_TIMER_ID)
                        {
                            fsm.set_state(new(fsm) IdleState(fsm));
                        }
                    }
                }
            }
        }
    }
}
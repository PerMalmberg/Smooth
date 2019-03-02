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
                        fsm.get_mqtt().send_packet(disconnect);
                        elapsed_time.start();
                    }

                    void DisconnectState::event(const core::network::TransmitBufferEmptyEvent&)
                    {
                        // Disconnect sent, we're done
                        fsm.set_state(new(fsm) IdleState(fsm));
                    }

                    void DisconnectState::tick()
                    {
                        if(elapsed_time.get_running_time() > std::chrono::seconds(2))
                        {
                            // Timeout, move on
                            fsm.set_state(new(fsm) IdleState(fsm));
                        }
                    }


                }
            }
        }
    }
}
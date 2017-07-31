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
                    }
                }
            }
        }
    }
}
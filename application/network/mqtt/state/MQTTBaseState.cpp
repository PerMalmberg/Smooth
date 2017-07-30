//
// Created by permal on 7/30/17.
//

#include <smooth/application/network/mqtt/state/MQTTBaseState.h>

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
                    MQTTBaseState::MQTTBaseState(MqttFSM& fsm)
                            : fsm(fsm)
                    {
                    }
                }
            }
        }
    }
}
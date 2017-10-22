//
// Created by permal on 7/31/17.
//

#pragma once

#include "DisconnectedState.h"


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
                    class IdleState
                            : public DisconnectedState
                    {
                        public:
                            IdleState(MqttFSM<MQTTBaseState>& fsm)
                                    : DisconnectedState(fsm, "IdleState")
                            {
                            }
                    };
                }
            }
        }
    }
}

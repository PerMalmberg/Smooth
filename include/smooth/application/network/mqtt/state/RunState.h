//
// Created by permal on 7/30/17.
//

#pragma once

#include "ConnectedState.h"

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
                    class RunState
                            : public ConnectedState
                    {
                        public:
                            RunState(MqttFSM<MQTTBaseState>& fsm)
                                    : ConnectedState(fsm, "RunState")
                            {
                            }

                            void tick() override;
                    };
                }
            }
        }
    }
}

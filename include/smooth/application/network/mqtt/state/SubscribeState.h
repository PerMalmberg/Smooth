//
// Created by permal on 7/30/17.
//

#pragma once

#include "MQTTBaseState.h"
#include "esp_log.h"

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
                    class SubscribeState
                            : public MQTTBaseState
                    {
                        public:
                            SubscribeState(MqttFSM<MQTTBaseState>& fsm)
                                    : MQTTBaseState(fsm, "SubscribeState")
                            {
                            }


                    };
                }
            }
        }
    }
}

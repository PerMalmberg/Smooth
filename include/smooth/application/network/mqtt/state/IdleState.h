//
// Created by permal on 7/31/17.
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
                    class IdleState
                            : public MQTTBaseState
                    {
                        public:
                            IdleState(MqttFSM<MQTTBaseState>& fsm)
                                    : MQTTBaseState(fsm, "IdleState")
                            {
                            }

                            void message( const core::network::ConnectionStatusEvent& msg) override;
                    };
                }
            }
        }
    }
}

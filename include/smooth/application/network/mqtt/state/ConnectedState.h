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
                    class ConnectedState
                            : public MQTTBaseState
                    {
                        public:
                            ConnectedState(MqttFSM<MQTTBaseState>& fsm, const char* name)
                                    : MQTTBaseState(fsm, name)
                            {
                            }

                            void message( const core::timer::TimerExpiredEvent& msg) override;
                    };
                }
            }
        }
    }
}

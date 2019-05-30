//
// Created by permal on 7/30/17.
//

#pragma once

#include "MQTTBaseState.h"

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

                            void event(const core::timer::TimerExpiredEvent& event) override;
                            void event(const core::network::event::ConnectionStatusEvent& event) override;
                    };
                }
            }
        }
    }
}

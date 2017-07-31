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
                    class ConnectingState
                            : public MQTTBaseState
                    {
                        public:
                            ConnectingState(MqttFSM<MQTTBaseState>& fsm)
                                    : MQTTBaseState(fsm, "Connecting")
                            {
                            }

                            void enter_state() override;

                            void message( const core::timer::TimerExpiredEvent& msg) override;

                            void receive(packet::ConnAck& conn_ack) override;

                    };
                }
            }
        }
    }
}

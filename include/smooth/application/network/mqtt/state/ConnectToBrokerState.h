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
                    class ConnectToBrokerState
                            : public ConnectedState
                    {
                        public:
                            ConnectToBrokerState(MqttFSM<MQTTBaseState>& fsm)
                                    : ConnectedState(fsm, "Connecting")
                            {
                            }

                            void enter_state() override;

                            void receive(packet::ConnAck& conn_ack) override;

                    };
                }
            }
        }
    }
}

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
                            explicit ConnectToBrokerState(MqttFSM<MQTTBaseState>& owner)
                                    : ConnectedState(owner, "ConnectToBroker")
                            {
                            }

                            void enter_state() override;

                            void receive(packet::ConnAck& conn_ack) override;
                        private:
                            bool is_using_clean_session = true;
                    };
                }
            }
        }
    }
}

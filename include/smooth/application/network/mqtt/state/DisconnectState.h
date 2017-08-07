//
// Created by permal on 7/30/17.
//

#pragma once

#include "ConnectedState.h"
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
                    class DisconnectState
                            : public ConnectedState
                    {
                        public:
                            DisconnectState(MqttFSM<MQTTBaseState>& fsm)
                                    : ConnectedState(fsm, "DisconnectState")
                            {
                            }

                            void enter_state() override;
                            void event(const core::network::TransmitBufferEmptyEvent& event) override;
                            void event(const core::timer::TimerExpiredEvent& event) override;
                    };
                }
            }
        }
    }
}

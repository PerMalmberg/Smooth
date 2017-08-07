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
                    class DisconnectedState
                            : public MQTTBaseState
                    {
                        public:
                            DisconnectedState(MqttFSM<MQTTBaseState>& fsm, const char* name)
                                    : MQTTBaseState(fsm, name)
                            {
                            }

                            void enter_state() override;

                            void event(const core::timer::TimerExpiredEvent& event) override;
                            void event(const core::network::ConnectionStatusEvent& event) override;
                        private:
                    };
                }
            }
        }
    }
}

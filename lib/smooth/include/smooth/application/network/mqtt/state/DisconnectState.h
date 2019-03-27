//
// Created by permal on 7/30/17.
//

#pragma once

#include "ConnectedState.h"
#include <smooth/core/timer/ElapsedTime.h>

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
                            explicit DisconnectState(MqttFSM<MQTTBaseState>& fsm)
                                    : ConnectedState(fsm, "DisconnectState")
                            {
                            }

                            void enter_state() override;
                            void event(const core::network::event::TransmitBufferEmptyEvent& event) override;
                            void tick() override;
                        private:
                            smooth::core::timer::ElapsedTime elapsed_time{};
                    };
                }
            }
        }
    }
}

//
// Created by permal on 8/6/17.
//

#pragma once

#include "DisconnectedState.h"

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
                    class StartupState
                            : public DisconnectedState
                    {
                        public:
                            explicit StartupState(MqttFSM<MQTTBaseState>& fsm)
                                    : DisconnectedState(fsm, "StartupState")
                            {
                            }

                            void enter_state() override
                            {
                                // Prevent automatic connection by overriding entry method
                            }
                    };
                }
            }
        }
    }
}
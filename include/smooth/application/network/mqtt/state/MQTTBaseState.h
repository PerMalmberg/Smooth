//
// Created by permal on 7/30/17.
//

#pragma once

#include <smooth/core/fsm/StaticFSM.h>

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
                    class MQTTBaseState;

                    typedef smooth::core::fsm::StaticFSM<MQTTBaseState, 100> MqttFSM;

                    class MQTTBaseState
                    {
                        public:
                            explicit MQTTBaseState(MqttFSM& fsm);

                            virtual ~MQTTBaseState() {}

                            void EnterState()
                            {
                            }

                            void LeaveState()
                            {
                            }

                        protected:
                            MqttFSM& fsm;
                    };
                }
            }
        }
    }
}

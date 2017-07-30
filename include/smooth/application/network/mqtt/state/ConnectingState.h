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
                            ConnectingState(MqttFSM& fsm)
                                    : MQTTBaseState(fsm)
                            {
                                ESP_LOGD("ConnectingState", "ConnectingState");
                            }

                            ~ConnectingState()
                            {
                                ESP_LOGD("ConnectingState", "~ConnectingState");
                            }
                    };
                }
            }
        }
    }
}

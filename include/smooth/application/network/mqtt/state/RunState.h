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
                    class RunState
                            : public ConnectedState
                    {
                        public:
                            RunState(MqttFSM<MQTTBaseState>& fsm)
                                    : ConnectedState(fsm, "RunState")
                            {
                            }

                            void tick() override;

                            void receive(packet::PubAck& pub_ack) override;

                            void receive(packet::PubRec& pub_rec) override;

                            void receive(packet::PubRel& pub_rel) override;

                            void receive(packet::PubComp& pub_comp) override;
                    };
                }
            }
        }
    }
}

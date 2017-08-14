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
                            RunState(MqttFSM<MQTTBaseState>& fsm, bool session_exists_on_server)
                                    : ConnectedState(fsm, "RunState"),
                                      session_exists_on_server(session_exists_on_server)
                            {
                            }

                            void tick() override;

                            // For publishing
                            void receive(packet::PubAck& pub_ack) override;
                            void receive(packet::PubRec& pub_rec) override;
                            void receive(packet::PubComp& pub_comp) override;

                            // For subscribing
                            void receive(packet::Publish& publish) override;
                            void receive(packet::SubAck& sub_ack) override;
                            void receive(packet::UnsubAck& unsub_ack) override;
                            void receive(packet::PubRel& pub_rel) override;

                        private:
                            bool session_exists_on_server;
                    };
                }
            }
        }
    }
}

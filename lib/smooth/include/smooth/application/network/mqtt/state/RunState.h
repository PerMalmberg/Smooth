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
                            RunState(MqttFSM<MQTTBaseState>& owner, bool clean_session_on_connect)
                                    : ConnectedState(owner, "RunState"),
                                      reconnect_handled(false),
                                      clean_session(clean_session_on_connect)
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
                            bool reconnect_handled;
                            bool clean_session;
                    };
                }
            }
        }
    }
}
